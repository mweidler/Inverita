/**
 * ProgressDialogUI.cpp
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

#include <QtCore>
#include <QVBoxLayout>
#include <QPushButton>

#include "ProgressDialogUI.h"

ProgressDialogUI::ProgressDialogUI(WorkerEngine *model, DialogType type, QWidget *parent) : QDialog(parent)
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
    m_progressBar->setMaximum(1000);

    m_labelRemaining = new QLabel();

    m_timer = new QTimer(this);
    m_timer->setInterval(500);

    m_buttonBox = new QDialogButtonBox();
    m_buttonBox->addButton(QDialogButtonBox::Abort);
    m_buttonBox->addButton(QDialogButtonBox::Ok);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(20, 20, 20, 20);
    //layout->setSpacing(20);
    for (int i = 0; i < m_model->taskCount(); i++) {
        QLabel *labelStep =  new QLabel(m_model->taskDescription(i));
        m_labelList.append(labelStep);
        layout->addWidget(labelStep);
    }
    layout->addWidget(m_progressBar);
    layout->addWidget(m_labelRemaining);
    if (type == ShowTextBox) {
        layout->addWidget(m_textArea);
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

    setMinimumSize(700, 400);
    resize(700, 400);
    setWindowTitle(tr("Progress"));

    m_previousCurrentTask = -1;
}

ProgressDialogUI::~ProgressDialogUI()
{

}

void ProgressDialogUI::display(QString message)
{
    m_textArea->insertHtml(message);
}

void ProgressDialogUI::finalize()
{
    m_timer->stop();
    m_buttonBox->button(QDialogButtonBox::Abort)->hide();
    m_buttonBox->button(QDialogButtonBox::Ok)->show();
    update();
}


/*!\reimp */
void ProgressDialogUI::showEvent(QShowEvent *event)
{
    m_buttonBox->button(QDialogButtonBox::Ok)->hide();
    m_buttonBox->button(QDialogButtonBox::Abort)->show();
    m_textArea->clear();
    m_statusHistory.clear();

    QDialog::showEvent(event);
    m_timer->start();
}

/*!\reimp */
void ProgressDialogUI::closeEvent(QCloseEvent *event)
{
    m_timer->stop();
    QDialog::closeEvent(event);
}


void ProgressDialogUI::update()
{
    QString remainingInfo;
    QString transferRateInfo;

    if (m_previousCurrentTask != m_model->currentTask()) {
        for (int i = 0; i < m_model->taskCount(); i++) {
            QString text = m_model->taskDescription(i);
            if (i == m_model->currentTask()) {
                text = "<b>" + text + "</b>";
            }
            m_labelList[i]->setText(text);
        }

        m_previousCurrentTask = m_model->currentTask();
    }


    while (m_statusHistory.size() >= 20) {
        m_statusHistory.removeFirst();
    }

    m_statusHistory.append(m_model->status());
    qreal completion = m_statusHistory.last().completion;

    if (m_statusHistory.size() >= 20) {
        qint64 deltaTransfered = m_statusHistory.last().processed - m_statusHistory.first().processed;
        qint64 deltaTimeMs = m_statusHistory.first().timestamp.msecsTo(m_statusHistory.last().timestamp);
        qreal  transferRate = deltaTransfered / (deltaTimeMs * 1000.0);
        transferRateInfo.sprintf(" %s %.1f MByte/sec",
                                 tr("while processing").toStdString().c_str(), transferRate);

        qreal deltaCompletion = completion - m_statusHistory.first().completion;
        qreal openCompletion = 1.0 - completion;
        qreal remainingSeconds = ((openCompletion / deltaCompletion) * deltaTimeMs) / 1000.0;
        if (remainingSeconds >= 60 * 60) {
            remainingInfo.sprintf("%.1f ", (float)(remainingSeconds / 60.0 / 60.0));
            remainingInfo += tr("hours remaining");
        } else if (remainingSeconds >= 120) {
            remainingInfo.sprintf("%d ", (int)(remainingSeconds / 60));
            remainingInfo += tr("minutes remaining");
        } else {
            remainingInfo.sprintf("%d ", (int)remainingSeconds);
            remainingInfo += tr("seconds remaining");
        }
    } else {
        remainingInfo = tr("Please be patient...");
    }

    if (completion >= 1.0) {
        remainingInfo = tr("Finished.");
        transferRateInfo.clear();
    }

    m_progressBar->setValue((int)(completion * m_progressBar->maximum()));
    m_labelRemaining->setText(remainingInfo + transferRateInfo);
}
