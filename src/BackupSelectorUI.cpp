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

#include <QFileDialog>


/*! Create a new BackupSelectorUI component with GUI elements
 */
BackupSelectorUI::BackupSelectorUI(QAbstractListModel *model, QWidget *parent) : QWidget(parent)
{
    m_choice = new QComboBox();
    m_choice->setModel(model);
    m_choice->setCurrentIndex(0);

    QLabel *labelCurrentBackup = new QLabel(tr("Select your backup (target and configuration) you want to work with or<br>"
                                            "open an existing backup or create a new backup from the menu.") + "<br><br>" +
                                            tr("Your current backup target:")
                                           );

    QPushButton *btnConf   = new QPushButton(tr("Configure"));
    btnConf->setIcon(QIcon::fromTheme("preferences-desktop"));

    QHBoxLayout *choiceLayout = new QHBoxLayout;
    choiceLayout->addWidget(m_choice, 1);
    choiceLayout->addWidget(btnConf);

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
    connect(btnConf, SIGNAL(clicked()), this, SLOT(onConfigure()));
}

BackupSelectorUI::~BackupSelectorUI()
{

}

int BackupSelectorUI::currentSelection()
{
    return m_choice->currentIndex();
}


void BackupSelectorUI::appendChoiceUnique(const QString &filepath)
{
    // do not store a duplicate item
    for (int i = 0; i < m_choice->count(); i++) {
        if (m_choice->itemText(i).compare(filepath) == 0) {
            m_choice->setCurrentIndex(i);
            return;
        }
    }

    int count = m_choice->model()->rowCount();
    m_choice->model()->insertRow(count);
    QModelIndex index = m_choice->model()->index(count, 0);
    m_choice->model()->setData(index, filepath);
    m_choice->setCurrentIndex(count);
}


void BackupSelectorUI::onSelect()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Select an existing backup configuration..."));
    filedialog.setFileMode(QFileDialog::ExistingFile);
    filedialog.setNameFilter("inverita.conf");
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    QFileInfo fileinfo(filedialog.selectedFiles()[0]);
    appendChoiceUnique(fileinfo.absolutePath());
    emit backupSelected();
}


void BackupSelectorUI::onNew()
{
    Configuration config;
    ConfigurationDialog configDialog(config, this);
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    QString newLocation = configDialog.location();
    config.Save(newLocation + "/" + "inverita.conf");

    appendChoiceUnique(newLocation);
    emit backupSelected();
}


void BackupSelectorUI::onConfigure()
{
    QString currentLocation = m_choice->currentText();

    Configuration config;
    config.Load(currentLocation + "/" + "inverita.conf");

    ConfigurationDialog configDialog(config, this);
    configDialog.setLocation(currentLocation);
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    QString newLocation = configDialog.location();
    config.Save(newLocation + "/" + "inverita.conf");

    appendChoiceUnique(newLocation);
    emit backupSelected();
}
