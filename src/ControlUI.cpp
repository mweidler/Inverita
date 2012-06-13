/**
 * ControlUI.cpp
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


#include <QPushButton>
#include <QHBoxLayout>
#include <QToolButton>

#include "ControlUI.h"


/*******************************************************************************
* Create a new BackupSelectorUI component with GUI elements
*******************************************************************************/
ControlUI::ControlUI(QWidget *parent) : QFrame(parent)
{
    QLabel *description = new QLabel(tr("<b>Start</b> your backup, or verify an existing backup."));

    QToolButton *btnCreate = new QToolButton(this);
    btnCreate->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnCreate->setText(tr("Create new backup snapshot"));
    btnCreate->setIcon(QIcon(":/images/Copy-icon.png"));
    btnCreate->setIconSize(QSize(96, 96));

    QToolButton *btnVerify = new QToolButton(this);
    btnVerify->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnVerify->setText(tr("Verify last backup with origin"));
    btnVerify->setIcon(QIcon(":/images/Verify-icon.png"));
    btnVerify->setIconSize(QSize(96, 96));

    m_driveUsageUI = new DriveUsageUI(parent);

    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->setMargin(5);
    hboxlayout->addWidget(description, 1);
    hboxlayout->addWidget(m_driveUsageUI, 0);
    hboxlayout->addWidget(btnCreate, 1);
    hboxlayout->addWidget(btnVerify, 1);
    QFrame *buttonframe = new QFrame();
    buttonframe->setLayout(hboxlayout);

    QVBoxLayout *vboxlayout = new QVBoxLayout;
    vboxlayout->setMargin(5);
    vboxlayout->addWidget(description);
    vboxlayout->addWidget(buttonframe);
    this->setLayout(vboxlayout);

    setFrameStyle(QFrame::StyledPanel);

    connect(btnCreate, SIGNAL(clicked()), this, SIGNAL(backupStarted()));
    connect(btnVerify, SIGNAL(clicked()), this, SIGNAL(startVerify()));
}

ControlUI::~ControlUI()
{

}
