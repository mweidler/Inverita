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

#include "ControlUI.h"


/*******************************************************************************
* Create a new BackupSelectorUI component with GUI elements
*******************************************************************************/
ControlUI::ControlUI(QWidget *parent) : QFrame(parent)
{
    QLabel *description = new QLabel(tr("<b>Start</b> your backup, or verify an existing backup."));
    QPushButton *btnCreate = new QPushButton(tr("Create\nnew\nBackup"), this);
    QPushButton *btnVerify = new QPushButton(tr("&Verify\nexisting\nBackup"), this);

    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->setMargin(5);
    hboxlayout->addWidget(description, 1);
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
}

ControlUI::~ControlUI()
{

}
