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

#include <iostream>

#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "ProgressDialogUI.h"

ProgressDialogUI::ProgressDialogUI(WorkerEngine *model, QWidget *parent) : QDialog(parent)
{
    m_model = model;

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    m_textArea = new QTextEdit;
    m_textArea->setFont(font);
    m_textArea->setPlainText("Hallo");

    m_progressBar = new QProgressBar(parent);
    m_progressBar->setTextVisible(true);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(1000);

    m_labelRemaining = new QLabel();

    m_timer = new QTimer(this);
    m_timer->setInterval(250);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Abort);

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
    layout->addWidget(m_textArea);
    layout->addWidget(buttonBox);
    this->setLayout(layout);

    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(rejected()), this, SIGNAL(aborted()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(m_model, SIGNAL(started()), this, SLOT(show()));
    connect(m_model, SIGNAL(aborted()), this, SLOT(close()));
    connect(m_model, SIGNAL(finished()), this, SLOT(close()));

    setMinimumSize(500, 300);
    resize(500, 300);
    setWindowTitle(tr("Progress"));

    m_previousCurrentTask = -1;
}

ProgressDialogUI::~ProgressDialogUI()
{

}

/*!\reimp */
void ProgressDialogUI::showEvent(QShowEvent *event)
{
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

    m_progressBar->setValue((int)(m_model->completion() * m_progressBar->maximum()));

    QString remaining;
    if (m_model->remainingSeconds() >= 60 * 60) {
        remaining.sprintf("%.1f ", (float)(m_model->remainingSeconds() / 60.0 / 60.0));
        remaining += tr("hours remaining");
    } else if (m_model->remainingSeconds() >= 120) {
        remaining.sprintf("%d ", m_model->remainingSeconds() / 60);
        remaining += tr("minutes remaining");
    } else {
        remaining.sprintf("%d ", m_model->remainingSeconds());
        remaining += tr("seconds remaining");
    }

    m_labelRemaining->setText(remaining);
}
