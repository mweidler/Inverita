/*
 * BackupSelectorUI.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2014  Marc Weidler <marc.weidler@web.de>,
 *                          Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                          All rights reserved.
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

#include <QLabel>
#include <QBoxLayout>
#include <QApplication>

/*! Create a new BackupSelectorUI component with GUI elements
 *
 * \param model the underlying backup list model (displayed in combobox)
 * \param parent the parent GUI element of this widget
 */
BackupSelectorUI::BackupSelectorUI(BackupListModel *model, QWidget *parent) : QWidget(parent)
{
    m_model = model;
    m_choice = new QComboBox();
    m_choice->setModel(model);
    m_choice->setCurrentIndex(-1);

    QLabel *description = new QLabel(
        tr("Select a recently used backup from the list below, or use the menu "
           "to open an existing backup or create a new backup.")
    );

    description->setWordWrap(true);

    m_btnConf = new QPushButton(tr("Configure"));
    m_btnConf->setIcon(QIcon::fromTheme("preferences-desktop"));
    m_btnConf->setEnabled(false);

    QHBoxLayout *choiceLayout = new QHBoxLayout;
    choiceLayout->addWidget(m_choice, 1);
    choiceLayout->addWidget(m_btnConf);

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setAlignment(Qt::AlignCenter);
    controlLayout->addWidget(description);
    controlLayout->addWidget(new QLabel(tr("Your current backup:")));
    controlLayout->addLayout(choiceLayout);

    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(QApplication::windowIcon().pixmap(96, 96));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(labelImage);
    mainLayout->addSpacerItem(new QSpacerItem(20, 0));
    mainLayout->addLayout(controlLayout);
    this->setLayout(mainLayout);

    connect(m_choice, SIGNAL(currentIndexChanged(int)), this, SIGNAL(backupSelected(int)));
    connect(m_btnConf, SIGNAL(clicked()), this, SIGNAL(configure()));
}


/*! Destructor
 */
BackupSelectorUI::~BackupSelectorUI()
{

}


int BackupSelectorUI::currentSelection()
{
    return m_choice->currentIndex();
}


/*! Select the given index in the combobox selector.
 *
 * This forces a currentIndexChanged event and results in reload of whole parameters.
 */
void BackupSelectorUI::select(int selection)
{
    return m_choice->setCurrentIndex(selection);
}


/*! Controls if the config button is enabled and the user can configure the backup.
 *
 * The button should only be enabled, if a valid backup is opened.
 */
void BackupSelectorUI::setEnableConfiguration(bool enabled)
{
    m_btnConf->setEnabled(enabled);
}

