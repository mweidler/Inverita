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

#include <QtGui>

#include "BackupSelectorUI.h"
#include "ConfigurationUI.h"


/*! Create a new BackupSelectorUI component with GUI elements
 */
BackupSelectorUI::BackupSelectorUI(QAbstractListModel *model, QWidget *parent) : QFrame(parent)
{
    m_choice = new QComboBox();
    m_choice->setModel(model);
    m_choice->setCurrentIndex(0);

    QLabel *labelCurrentBackup = new QLabel(tr("Current backup:"));
    QHBoxLayout *choiceLayout = new QHBoxLayout;
    choiceLayout->setMargin(5);
    choiceLayout->addWidget(labelCurrentBackup);
    choiceLayout->addWidget(m_choice, 1);

    QPushButton *btnSelect = new QPushButton(tr("Select existing"));
    btnSelect->setIcon(QIcon::fromTheme("find"));
    QPushButton *btnNew    = new QPushButton(tr("Create new"));
    btnNew->setIcon(QIcon::fromTheme("document-new"));
    QPushButton *btnConf   = new QPushButton(tr("Configure"));
    btnConf->setIcon(QIcon::fromTheme("preferences-desktop"));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setMargin(5);
    buttonLayout->setSpacing(20);
    buttonLayout->addWidget(btnSelect);
    buttonLayout->addWidget(btnNew);
    buttonLayout->addWidget(btnConf);

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setSizeConstraint(QLayout::SetMaximumSize);
    controlLayout->setMargin(5);
    controlLayout->addLayout(choiceLayout, 1);
    controlLayout->addLayout(buttonLayout, 1);

    QPixmap pixmap(":/images/backup-icon.png");
    //QIcon icon = QIcon::fromTheme("deja-dup").pixmap(96,96);
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(pixmap);

    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->setMargin(5);
    hboxlayout->addWidget(labelImage);
    hboxlayout->addLayout(controlLayout);

    QLabel *description = new QLabel(tr("<b>Select</b> your backup, create a <b>new</b> or <b>configure</b> an existing backup."));

    QVBoxLayout *vboxlayout = new QVBoxLayout;
    vboxlayout->setMargin(5);
    vboxlayout->addWidget(description);
    vboxlayout->addLayout(hboxlayout);
    this->setLayout(vboxlayout);

    setFrameStyle(QFrame::StyledPanel);

    connect(m_choice, SIGNAL(currentIndexChanged(int)), this, SIGNAL(backupSelected()));
    connect(btnSelect, SIGNAL(clicked()), this, SLOT(onSelect()));
    connect(btnNew, SIGNAL(clicked()), this, SLOT(onNew()));
    connect(btnConf, SIGNAL(clicked()), this, SLOT(onConfigure()));
}

BackupSelectorUI::~BackupSelectorUI()
{

}

int BackupSelectorUI::currentSelection()
{
    return m_choice->currentIndex();
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
    QString dirname = fileinfo.absolutePath();

    // do not store a duplicate item
    for (int i = 0; i < m_choice->count(); i++) {
        if (m_choice->itemText(i).compare(dirname) == 0) {
            m_choice->setCurrentIndex(i);
            emit backupSelected();
            return;
        }
    }

    int count = m_choice->model()->rowCount();
    m_choice->model()->insertRow(count);
    QModelIndex index = m_choice->model()->index(count, 0);
    m_choice->model()->setData(index, dirname);

    m_choice->setCurrentIndex(count);
    emit backupSelected();
}


void BackupSelectorUI::onNew()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Specify a new backup..."));
    filedialog.setFileMode(QFileDialog::Directory);
    filedialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    QString dirname = filedialog.selectedFiles()[0];
    QString filename = dirname + "/" + "inverita.conf";

    Configuration config;
    ConfigurationUI configUI(config, this);
    if (configUI.exec() == QDialog::Accepted) {
        config.Save(filename);

        // do not store a duplicate item
        for (int i = 0; i < m_choice->count(); i++) {
            if (m_choice->itemText(i).compare(dirname) == 0) {
                m_choice->setCurrentIndex(i);
                emit backupSelected();
                return;
            }
        }

        int count = m_choice->model()->rowCount();
        m_choice->model()->insertRow(count);
        QModelIndex index = m_choice->model()->index(count, 0);
        m_choice->model()->setData(index, dirname);

        m_choice->setCurrentIndex(count);
        emit backupSelected();
    }
}

void BackupSelectorUI::onConfigure()
{
    QString dirname = m_choice->currentText();
    QString filename = dirname + "/" + "inverita.conf";

    Configuration config;
    config.Load(filename);

    ConfigurationUI configUI(config, this);
    if (configUI.exec() == QDialog::Accepted) {
        config.Save(filename);
        emit backupSelected();
    }
}
