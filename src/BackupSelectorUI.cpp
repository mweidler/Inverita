/*
 * BackupSelectorUI.cpp
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


#include "BackupSelectorUI.h"
#include "ConfigurationDialog.h"
#include "PasswordDialog.h"

#include <QLabel>
#include <QBoxLayout>


/*! Create a new BackupSelectorUI component with GUI elements
 */
BackupSelectorUI::BackupSelectorUI(BackupListModel *model, QWidget *parent) : QWidget(parent)
{
    m_model = model;
    m_choice = new QComboBox();
    m_choice->setModel(model);
    m_choice->setCurrentIndex(-1);

    QLabel *labelCurrentBackup = new QLabel(tr("Select your backup (target and configuration) you want to work with or<br>"
                                            "open an existing backup or create a new backup from the menu.") + "<br><br>" +
                                            tr("Your current backup target:")
                                           );

    m_btnConf = new QPushButton(tr("Configure"));
    m_btnConf->setIcon(QIcon::fromTheme("preferences-desktop"));
    m_btnConf->setEnabled(false);

    QHBoxLayout *choiceLayout = new QHBoxLayout;
    choiceLayout->addWidget(m_choice, 1);
    choiceLayout->addWidget(m_btnConf);

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setAlignment(Qt::AlignCenter);
    controlLayout->addWidget(labelCurrentBackup);
    controlLayout->addLayout(choiceLayout);

    QPixmap pixmap(":/images/backup-icon.png");
    //QIcon icon = QIcon::fromTheme("deja-dup").pixmap(96,96);
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(pixmap);

    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->addWidget(labelImage);
    hboxlayout->addSpacerItem(new QSpacerItem(20, 0));
    hboxlayout->addLayout(controlLayout);
    this->setLayout(hboxlayout);

    connect(m_choice, SIGNAL(currentIndexChanged(int)), this, SIGNAL(backupSelected()));
    connect(m_btnConf, SIGNAL(clicked()), this, SIGNAL(configure()));
}

BackupSelectorUI::~BackupSelectorUI()
{

}


void BackupSelectorUI::select(int index)
{
    return m_choice->setCurrentIndex(index);
}


int BackupSelectorUI::currentSelection()
{
    return m_choice->currentIndex();
}


void BackupSelectorUI::setEnableConfiguration(bool enabled)
{
    m_btnConf->setEnabled(enabled);
}

