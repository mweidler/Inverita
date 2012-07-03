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
#include <QInputDialog>
#include <QFileDialog>


/*! Create a new BackupSelectorUI component with GUI elements
 */
BackupSelectorUI::BackupSelectorUI(BackupListModel *model, QWidget *parent) : QWidget(parent)
{
    m_model = model;
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

    connect(m_choice, SIGNAL(currentIndexChanged(int)), this, SLOT(onChange()));
    connect(btnConf, SIGNAL(clicked()), this, SLOT(onConfigure()));
}

BackupSelectorUI::~BackupSelectorUI()
{

}

const BackupEntry BackupSelectorUI::currentBackup()
{
    return m_currentBackup;
}


void BackupSelectorUI::unmountEncfs()
{
    QDir dir;
    if (dir.exists("/tmp/encfsinverita")) {
        system("fusermount -u /tmp/encfsinverita");
        dir.remove("/tmp/encfsinverita");
    }
}


void BackupSelectorUI::mountEncfs(const QString &origin)
{
    m_currentBackup.origin = origin;
    m_currentBackup.encrypted = false;
    m_currentBackup.password = "";
    m_currentBackup.location = origin;

    if (QFile::exists(origin + "/" + ".encfs6.xml")) {
        m_currentBackup.encrypted = true;

        QInputDialog passwordDialog(this);
        passwordDialog.setLabelText(tr("Please enter the password"));
        passwordDialog.setMinimumSize(640, 480);
        if (passwordDialog.exec() == QDialog::Accepted) {
            m_currentBackup.password = passwordDialog.textValue();
            m_currentBackup.location = "/tmp/encfsinverita";
            QDir dir;
            dir.mkpath(m_currentBackup.location);
            QString sysstring = "echo \"" + m_currentBackup.password + "\" | encfs -S " + m_currentBackup.origin + " " + m_currentBackup.location;
            system(sysstring.toUtf8().data());
        }
    }
}


void BackupSelectorUI::appendChoiceUnique(BackupEntry entry)
{
    // do not store a duplicate item
    for (int i = 0; i < m_choice->count(); i++) {
        if (m_choice->itemText(i).compare(entry.origin) == 0) {
            m_choice->setCurrentIndex(i);
            return;
        }
    }

    m_model->appendEntry(entry);
    m_model->Save();
    m_choice->setCurrentIndex(m_model->size());
}


void BackupSelectorUI::onSelect()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Select an existing backup configuration..."));
    filedialog.setFileMode(QFileDialog::ExistingFile);
    filedialog.setNameFilter("inverita.conf .encfs6.xml");
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    QFileInfo fileinfo(filedialog.selectedFiles()[0]);
    QString origin = fileinfo.absolutePath();
    mountEncfs(origin);

    appendChoiceUnique(m_currentBackup);
    emit backupSelected();
}


void BackupSelectorUI::onNew()
{
    Configuration config;
    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Create new backup configuration"));
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    QString origin = configDialog.location();
    mountEncfs(origin);
    config.Save(m_currentBackup.location + "/" + "inverita.conf");

    appendChoiceUnique(m_currentBackup);
    emit backupSelected();
}


void BackupSelectorUI::onConfigure()
{
    QString origin = m_choice->currentText();
    mountEncfs(origin);

    Configuration config;
    config.Load(m_currentBackup.location + "/" + "inverita.conf");

    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Configuring backup") + "' " + origin + "'");
    configDialog.setLocation(m_currentBackup.location);
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    QString newOrigin = configDialog.location();
    if (origin.compare(newOrigin) != 0) {
        unmountEncfs();
        mountEncfs(newOrigin);
    }

    config.Save(m_currentBackup.location + "/" + "inverita.conf");

    appendChoiceUnique(m_currentBackup);
    emit backupSelected();
}

void BackupSelectorUI::onChange()
{
    unmountEncfs();

    QString origin = m_choice->currentText();
    mountEncfs(origin);

    emit backupSelected();
}
