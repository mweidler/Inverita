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

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
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

    connect(m_choice, SIGNAL(currentIndexChanged(int)), this, SLOT(onChange()));
    connect(m_btnConf, SIGNAL(clicked()), this, SLOT(onConfigure()));
}

BackupSelectorUI::~BackupSelectorUI()
{

}

int BackupSelectorUI::currentSelection()
{
    return m_choice->currentIndex();
}


void BackupSelectorUI::unmountEncfs()
{
    QDir dir("/tmp/encfsinverita");
    if (dir.exists()) {
        QProcess encfs;
        encfs.start("fusermount", QStringList() << "-u" << dir.absolutePath());
        encfs.waitForStarted();
        encfs.waitForFinished();

        QByteArray result = encfs.readAllStandardError();
        int rc = encfs.exitCode();

        if (rc != 0) {
            QString title = tr("The encrypted backup target can not be closed") + ":\n" + dir.absolutePath() + "\n\n";
            QString encfsCode = QString("fusermount code: %1").arg(rc);
            QString msg = title + QString::fromLocal8Bit(result) + "\n" + encfsCode;
            QMessageBox::critical(0, tr("INVERITA backup target error"), msg);
        }

        dir.rmdir(dir.absolutePath());
    }
}


void BackupSelectorUI::mountEncfs(int index)
{
    BackupEntry entry = m_model->at(index);

    if (isEnfcsDirectory(entry.origin)) {
        unmountEncfs();

        QString currentPassword = entry.password;
        PasswordDialog passwordDialog(this);
        passwordDialog.setPassword(currentPassword);
        passwordDialog.setRememberPassword(!currentPassword.isEmpty());
        if (passwordDialog.exec() == QDialog::Rejected) {
            return;
        }

        currentPassword = passwordDialog.password();
        if (passwordDialog.rememberPassword()) {
            entry.password = currentPassword;
        } else {
            entry.password.clear();
        }

        m_model->setEntry(entry);

        setCursor(Qt::WaitCursor);

        QDir dir;
        dir.mkpath(entry.location);

        QProcess encfs;
        encfs.start("encfs", QStringList() << "-S" <<  entry.origin << entry.location);
        encfs.waitForStarted();
        encfs.write(entry.password.toUtf8());
        encfs.closeWriteChannel();
        encfs.waitForFinished();

        unsetCursor();

        QByteArray resultStdOut = encfs.readAllStandardOutput();
        QByteArray resultStdErr = encfs.readAllStandardError();
        int rc = encfs.exitCode();

        if (rc != 0) {
            QString msg = tr("The encrypted backup target can not be opened") + ":\n" + entry.origin + "\n\n" +
                          QString::fromLocal8Bit(resultStdOut) +
                          QString::fromLocal8Bit(resultStdErr) + "\n" +
                          QString("encfs code: %1").arg(rc);
            QMessageBox::critical(0, tr("INVERITA backup target error"), msg);
        }
    }
}


bool BackupSelectorUI::isEnfcsDirectory(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::Hidden | QDir::Files);
    QFileInfoList filelist = dir.entryInfoList();

    for (int i = 0; i < filelist.count(); i++) {
        QFileInfo fileinfo = filelist[i];
        if (fileinfo.fileName().startsWith(".encfs")) {
            return true;
        }
    }

    return false;
}


void BackupSelectorUI::onSelect()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Select an existing backup configuration..."));
    filedialog.setFileMode(QFileDialog::ExistingFile);
    //filedialog.setFilter(QDir::AllEntries | QDir::Hidden);
    filedialog.setNameFilter("inverita.conf .encfs*");
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    QFileInfo fileinfo(filedialog.selectedFiles()[0]);

    BackupEntry entry;
    entry.origin = fileinfo.absolutePath();
    entry.location = entry.origin;
    entry.password.clear();

    if (isEnfcsDirectory(entry.origin)) {
        entry.location = QString("/tmp/encfsinverita");
    }

    int index = m_model->setEntry(entry);
    m_choice->setCurrentIndex(index); // causes a currentIndexChanged event
}


void BackupSelectorUI::onNew()
{
    Configuration config;
    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Create new backup configuration"));
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    BackupEntry entry;
    entry.origin = configDialog.location();
    entry.location = entry.origin;
    entry.password.clear();

    if (isEnfcsDirectory(entry.origin)) {
        entry.location = QString("/tmp/encfsinverita");
    }

    int index = m_model->setEntry(entry);
    mountEncfs(index);
    config.Save(entry.location + "/" + "inverita.conf");
    unmountEncfs();

    m_choice->setCurrentIndex(index); // causes a currentIndexChanged event
}


void BackupSelectorUI::onConfigure()
{
    BackupEntry entry = m_model->at(m_choice->currentIndex());

    Configuration config;
    config.Load(entry.location + "/" + "inverita.conf");

    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Configuring backup") + "' " + entry.origin + "'");
    configDialog.setLocation(entry.origin);
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    QString newOrigin = configDialog.location();
    if (entry.origin.compare(newOrigin) != 0) {
        entry.origin = newOrigin;
        entry.location = entry.origin;
        if (isEnfcsDirectory(entry.origin)) {
            entry.location = QString("/tmp/encfsinverita");
        }

        int index = m_model->setEntry(entry);
        mountEncfs(index);
        config.Save(entry.location + "/" + "inverita.conf");
        unmountEncfs();

        m_choice->setCurrentIndex(index); // causes a currentIndexChanged event
    } else {
        config.Save(entry.location + "/" + "inverita.conf");
    }
}


void BackupSelectorUI::onChange()
{
    qDebug() << "BackupSelectorUI::onChange()" << m_choice->currentIndex();
    m_btnConf->setEnabled(m_choice->currentIndex() >= 0 ? true : false);
    if (m_choice->currentIndex() >= 0) {
        unmountEncfs();
        mountEncfs(m_choice->currentIndex());
        emit backupSelected();
    }
}
