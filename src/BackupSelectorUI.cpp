/*
 * BackupSelectorUI.cpp
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

#include <QtGui>

#include "BackupSelectorUI.h"
#include "ConfigurationUI.h"

/*******************************************************************************
* Create a new BackupSelectorUI component with GUI elements
*******************************************************************************/
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
    filedialog.setNameFilter("alvara.conf");
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
    QString filename = dirname + "/" + "alvara.conf";

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
    QString filename = dirname + "/" + "alvara.conf";

    Configuration config;
    config.Load(filename);

    ConfigurationUI configUI(config, this);
    if (configUI.exec() == QDialog::Accepted) {
        config.Save(filename);
        emit backupSelected();
    }
}
