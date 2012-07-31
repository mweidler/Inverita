/*
 * ProgressDialog.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012  Marc Weidler <marc.weidler@web.de>,
 *                     Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                     All rights reserved.
 *
 * INVERITA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * INVERITA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ProgressDialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>


/*! Constructs a new Progress Dialog object with UI elements
 *
 * \param model  the model engine interface providing status information
 * \param type   dialog type
 * \param parent the parent ui element
 */
ProgressDialog::ProgressDialog(WorkerEngine *model, DialogType type, DialogAbortable abortable, QWidget *parent) : QDialog(parent)
{
    m_model = model;

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    //font.setPointSize(10);

    m_textArea = new QTextEdit;
    m_textArea->setReadOnly(true);
    m_textArea->setFont(font);

    m_progressBar = new QProgressBar(parent);
    m_progressBar->setTextVisible(true);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0);

    m_labelRemaining = new QLabel();

    m_timer = new QTimer(this);
    m_timer->setInterval(500);

    m_buttonBox = new QDialogButtonBox();
    m_buttonBox->addButton(QDialogButtonBox::Abort);
    m_buttonBox->addButton(QDialogButtonBox::Ok);
    m_buttonBox->button(QDialogButtonBox::Abort)->setIcon(QIcon::fromTheme("stop"));
    m_buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme("ok"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(20, 20, 20, 20);
    //layout->setSpacing(20);
    for (int i = 0; i < m_model->tasks(); i++) {
        QLabel *labelStep =  new QLabel(m_model->task(i));
        m_labelList.append(labelStep);
        layout->addWidget(labelStep);
    }
    layout->addWidget(m_progressBar);
    layout->addWidget(m_labelRemaining);

    switch (type) {
        case ProgressDialog::ShowTextBox:
            layout->addWidget(m_textArea);
            setMinimumSize(700, 400);
            break;

        default: // fall through
        case ProgressDialog::NoTextBox:
            setMinimumSize(600, 200);
            break;
    }

    switch (abortable) {
        case ProgressDialog::NotAbortable:
            m_buttonBox->setEnabled(false);
            break;

        default: // fall through
        case ProgressDialog::Abortable:
            m_buttonBox->setEnabled(true);
            break;
    }

    layout->addWidget(m_buttonBox);
    this->setLayout(layout);

    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SIGNAL(aborted()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(close()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(m_model, SIGNAL(started()), this, SLOT(show()));
    connect(m_model, SIGNAL(aborted()), this, SLOT(close()));
    connect(m_model, SIGNAL(finished()), this, SLOT(finalize()));

    setModal(true);
    m_previousCurrentTask = -1;
}


/*! Destructor
 */
ProgressDialog::~ProgressDialog()
{
    qDebug() << "ProgressDialog::~ProgressDialog()";
}


/*! Receives string messages to be displayed in the progress dialog text area
 *
 * \param message the message to be added to the dialog text area
 */
void ProgressDialog::display(QString message)
{
    m_textArea->insertHtml(message);
}


/*! Signals the user, that the job has finished and the dialog can be closed by the user.
 */
void ProgressDialog::finalize()
{
    m_timer->stop();
    m_buttonBox->setEnabled(true);
    m_buttonBox->button(QDialogButtonBox::Abort)->hide();
    m_buttonBox->button(QDialogButtonBox::Ok)->show();
    qDebug() << "Finalized update requested";
    update();
}


/*! Is called, if the dialog is shown to the user
 *
 * \param event the show event
 */
void ProgressDialog::showEvent(QShowEvent *event)
{
    m_buttonBox->button(QDialogButtonBox::Ok)->hide();
    m_buttonBox->button(QDialogButtonBox::Abort)->show();
    m_textArea->clear();
    m_statusHistory.clear();
    m_progressBar->reset();
    m_labelRemaining->clear();

    QDialog::showEvent(event);
    m_timer->start();
}


/*! Is called, if the dialog is about to close
 *
 * \param event the close event
 */
void ProgressDialog::closeEvent(QCloseEvent *event)
{
    m_timer->stop();
    QDialog::closeEvent(event);
}


/*! Update infos in the progress dialog
 */
void ProgressDialog::update()
{
    QString timeInfo;
    QString remainingInfo;
    QString transferRateInfo;

    if (m_previousCurrentTask != m_model->currentTask()) {
        for (int i = 0; i < m_model->tasks(); i++) {
            QString text = m_model->task(i);
            if (i == m_model->currentTask()) {
                text = "<b>" + text + "</b>";
            }
            m_labelList[i]->setText(text);
        }

        m_previousCurrentTask = m_model->currentTask();
    }


    while (m_statusHistory.size() >= 60) {
        m_statusHistory.removeFirst();
    }

    m_statusHistory.append(m_model->status());
    qreal completion = m_statusHistory.last().completion;
    if (completion > 0) {
        m_progressBar->setMaximum(1000);
    }

    if (m_statusHistory.size() >= 20) {
        qint64 deltaTransferred = m_statusHistory.last().transferred - m_statusHistory.first().transferred;
        qint64 deltaTimeMs = m_statusHistory.first().timestamp.msecsTo(m_statusHistory.last().timestamp);
        int transferRate = qRound(deltaTransferred / (deltaTimeMs * 1000.0));

        qreal deltaCompletion = completion - m_statusHistory.first().completion;
        qreal openCompletion = 1.0 - completion;
        int remainingSeconds = qRound(((openCompletion / deltaCompletion) * deltaTimeMs) / 1000.0);
        int remainingMinutes = remainingSeconds / 60.0;
        int remainingHours = remainingMinutes / 60.0;

        if (remainingHours >= 1) {
            timeInfo = QString("%1:%2").arg(remainingHours).arg(remainingMinutes, 2, 10, QChar('0'));
        } else if (remainingMinutes >= 1) {
            timeInfo = tr("%n minute(s)", "", remainingMinutes);
        } else {
            timeInfo = tr("%n second(s)", "", remainingSeconds);
        }

        if (transferRate > 0) {
            remainingInfo = tr("%1 estimated time remaining, processing %2 MByte/s").arg(timeInfo).arg(transferRate);
        } else {
            remainingInfo = tr("%1 estimated time remaining").arg(timeInfo);
        }
    } else {
        remainingInfo = tr("Please be patient...");
    }

    if (completion >= 1.0) {
        remainingInfo = tr("Finished.");
        transferRateInfo.clear();
    }

    if (completion > 1.0) {
        qDebug() << "Completion exceeds 1.0:" << completion;
    }

    m_progressBar->setValue((int)(completion * m_progressBar->maximum()));
    m_labelRemaining->setText(remainingInfo);
}
