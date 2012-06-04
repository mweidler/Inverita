/*
 * ProgressDialogUI.cpp
 *
 * Copyright (c) 2012, Marc Weidler <marc.weidler@web.de>
 * Ulrichstr. 12/1, 71672 Marbach, Germany
 * All rights reserved.
 *
 * 'ALVARA Personal Backup' is licensed under the BSD license (simplified, 2-clause).
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>

#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "ProgressDialogUI.h"

ProgressDialogUI::ProgressDialogUI(AlvaraEngine *model, QWidget *parent) : QDialog(parent)
{
    m_model = model;

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
