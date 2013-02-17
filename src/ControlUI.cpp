/*
 * ControlUI.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012 - 2013  Marc Weidler <marc.weidler@web.de>,
 *                            Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                            All rights reserved.
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


#include "ControlUI.h"

#include <QLabel>
#include <QBoxLayout>


/*!
 * Create a new ControlUI component with GUI elements
 *
 * \param parent the parent control element of this UI element
 */
ControlUI::ControlUI(QWidget *parent) : QWidget(parent)
{
    QLabel *description = new QLabel(tr("<b>Create</b> a new backup snapshot or <b>verify</b> your existing backup."));
    description->setWordWrap(true);

    m_btnCreate = new QToolButton(this);
    m_btnCreate->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_btnCreate->setText(tr("Create new backup snapshot"));
    m_btnCreate->setIcon(QIcon(":/images/copy-icon.png"));
    m_btnCreate->setIconSize(QSize(64, 64));

    m_btnVerify = new QToolButton(this);
    m_btnVerify->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_btnVerify->setText(tr("Verify last backup with origin"));
    m_btnVerify->setIcon(QIcon(":/images/verify-icon.png"));
    m_btnVerify->setIconSize(QSize(64, 64));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_btnCreate);
    buttonLayout->addSpacerItem(new QSpacerItem(15, 0));
    buttonLayout->addWidget(m_btnVerify);

    QVBoxLayout *masterLayout = new QVBoxLayout;
    masterLayout->setAlignment(Qt::AlignTop);
    masterLayout->addWidget(description);
    masterLayout->addLayout(buttonLayout);
    this->setLayout(masterLayout);

    connect(m_btnCreate, SIGNAL(clicked()), this, SIGNAL(startBackup()));
    connect(m_btnVerify, SIGNAL(clicked()), this, SIGNAL(startVerify()));
}


/*! Destructor
 */
ControlUI::~ControlUI()
{

}


/*! Enable/disables buttons of this control
 *
 * \param button the button to be disabled/enabled
 * \param enabled true enables button, false disables the button
 */
void ControlUI::setEnabledButtons(LogicalButton button, bool enabled)
{
    switch (button) {
        case ControlUI::CreateButton:
            m_btnCreate->setEnabled(enabled);
            break;

        case ControlUI::VerifyButton:
            m_btnVerify->setEnabled(enabled);
            break;

        case ControlUI::AllButtons:
            m_btnCreate->setEnabled(enabled);
            m_btnVerify->setEnabled(enabled);
            break;
    }
}
