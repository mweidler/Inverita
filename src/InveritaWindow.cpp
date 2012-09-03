/*
 * InveritaWindow.cpp
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


#include "InveritaWindow.h"
#include "AboutDialog.h"
#include "Utilities.h"
#include "Configuration.h"
#include "Backup.h"
#include "ConfigurationDialog.h"
#include "PasswordDialog.h"

#include <QBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QMenuBar>
#include <QDebug>


/*! Constructs a new mainwindow objects and initializes the user interface.
 *
 * \param parent the parent UI element
 */
InveritaWindow::InveritaWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    m_backupListModel = new BackupListModel(this);
    m_backupListModel->load("inverita");
    m_backupSelectorUI = new BackupSelectorUI(m_backupListModel, this);

    m_snapshotListModel = new SnapshotListModel(this);
    m_snapshotListUI = new SnapshotListUI(m_snapshotListModel, this);

    m_driveCapacityUI = new DriveCapacityUI(&m_filesystemInfo, this);
    m_controlUI = new ControlUI(this);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_driveCapacityUI);
    hlayout->addWidget(m_controlUI);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(20);
    layout->addWidget(m_backupSelectorUI);
    layout->addWidget(m_snapshotListUI);
    layout->addLayout(hlayout);
    widget->setLayout(layout);

    createActions();
    createMenus();

    // The Worker engine should run in its own thread to not
    // block the main event queue.
    m_backupEngine.moveToThread(&m_backupThread);
    m_backupThread.start(QThread::LowestPriority);

    m_eraseEngine.moveToThread(&m_eraseThread);
    m_eraseThread.start(QThread::LowestPriority);

    m_validateEngine.moveToThread(&m_validateThread);
    m_validateThread.start(QThread::LowestPriority);

    m_verifyEngine.moveToThread(&m_verifyThread);
    m_verifyThread.start(QThread::LowestPriority);

    m_progressBackupDialog = new ProgressDialog(&m_backupEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressBackupDialog->setWindowTitle(tr("Creating new backup snapshot..."));
    m_progressEraseDialog = new ProgressDialog(&m_eraseEngine, ProgressDialog::NoTextBox, ProgressDialog::NotAbortable, this);
    m_progressEraseDialog->setWindowTitle(tr("Deleting backup snapshot..."));
    m_progressValidateDialog = new ProgressDialog(&m_validateEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressValidateDialog->setWindowTitle(tr("Validating backup snapshot..."));
    m_progressVerifyDialog = new ProgressDialog(&m_verifyEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressVerifyDialog->setWindowTitle(tr("Verifying backup..."));

    connect(m_backupSelectorUI, SIGNAL(backupSelected(int)), this, SLOT(onBackupSelected(int)));
    connect(m_backupSelectorUI, SIGNAL(configure()), this, SLOT(onConfigure()));
    connect(m_snapshotListUI, SIGNAL(reload()), this, SLOT(reload()));
    connect(m_snapshotListUI, SIGNAL(deleteBackup()), this, SLOT(onDeleteBackup()));
    connect(m_snapshotListUI, SIGNAL(validateBackup()), this, SLOT(onValidateBackup()));

    connect(m_controlUI, SIGNAL(startBackup()), &m_backupEngine, SLOT(start()));
    connect(m_progressBackupDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(&m_backupEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(&m_backupEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(&m_backupEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));
    connect(&m_backupEngine, SIGNAL(report(QString)), m_progressBackupDialog, SLOT(display(QString)));

    connect(this, SIGNAL(deleteBackup()), &m_eraseEngine, SLOT(start()));
    connect(m_progressEraseDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(&m_eraseEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(&m_eraseEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(&m_eraseEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));

    connect(this, SIGNAL(validateBackup()), &m_validateEngine, SLOT(start()));
    connect(m_progressValidateDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(&m_validateEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(&m_validateEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(&m_validateEngine, SIGNAL(failed()), this, SLOT(onValidationFailed()));
    connect(&m_validateEngine, SIGNAL(report(QString)), m_progressValidateDialog, SLOT(display(QString)));

    connect(m_controlUI, SIGNAL(startVerify()), &m_verifyEngine, SLOT(start()));
    connect(m_progressVerifyDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(&m_verifyEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(&m_verifyEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(&m_verifyEngine, SIGNAL(failed()), this, SLOT(onVerificationFailed()));
    connect(&m_verifyEngine, SIGNAL(report(QString)), m_progressVerifyDialog, SLOT(display(QString)));

    connect(&m_timer, SIGNAL(timeout()), &m_filesystemInfo, SLOT(refresh()));

    m_controlUI->setEnabledButtons(ControlUI::AllButtons, false);
    m_timer.setInterval(5000);  // for filesystem capacity update
    m_timer.start();

    setWindowTitle(tr("INVERITA Personal Backup"));
    setMinimumSize(750, 600);
    resize(750, 600);

    // center application window on the current screen
    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    move(desktopRect.center() - rect().center());
}


/*! Destructor
 */
InveritaWindow::~InveritaWindow()
{
}


void InveritaWindow::closeEvent(QCloseEvent *event)
{
    m_backupThread.exit();
    m_eraseThread.exit();
    m_validateThread.exit();
    m_verifyThread.exit();

    m_backupThread.wait();
    m_eraseThread.wait();
    m_validateThread.wait();
    m_verifyThread.wait();

    closeCurrentBackup();
    QMainWindow::closeEvent(event);
}


/*! Is called if the user selects the "application about" in the menu.
 */
void InveritaWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}


void InveritaWindow::onBackupFailed()
{
    QString msg = tr("A critical error during backup execution has occured:") + "<br><br>";
    msg += m_backupEngine.failureHint() + "<br>";
    msg += tr("Make sure, you have proper privileges to write to the<br>"
              "backup medium. If this error occurs for the first time<br>"
              "on a long time used backup medium, please consider to<br>"
              "replace this medium with a new one.<br><br>"
              "Due to backup integrity, this backup snapshot with all<br>"
              "contained folders and files should be deleted manually.<br>"
              "Please try to solve the problem and restart the backup."
             );

    QMessageBox::critical(this, tr("Backup execution error"), msg);

    m_progressBackupDialog->hide();
    m_progressEraseDialog->hide();
    reload();
}


void InveritaWindow::onValidationFailed()
{
    m_progressValidateDialog->hide();

    QString msg = tr("A critical error during validation has occured:") + "<br><br>";
    msg += m_validateEngine.failureHint() + "<br>";
    QMessageBox::critical(this, tr("Snapshot validation error"), msg);

    reload();
}


void InveritaWindow::onVerificationFailed()
{
    m_progressVerifyDialog->hide();

    QString msg = tr("A critical error during verification has occured:") + "<br><br>";
    msg += m_verifyEngine.failureHint() + "<br>";
    QMessageBox::critical(this, tr("Backup verification error"), msg);

    reload();
}


void InveritaWindow::reload()
{
    QString location = Backup::instance().location();
    if (Backup::instance().isOpen()) {
        m_snapshotListModel->investigate(location);
        /* Filesystem infos must be gathered from the origin storage, not
           from an eventually virtual file system, like encfs. Otherwise, the
           file system type can not be determined correctly. */
        m_filesystemInfo.setFile(Backup::instance().origin());
        if (m_filesystemInfo.filesystemType() != FilesystemInfo::Ext4) {
            QString msg = tr("Your backup medium has an unsupported filesystem. "
                             "Only 'ext4' filesystems are supported.<br><br>"
                             "<b>You can continue at your own risk!</b>");
            QMessageBox::warning(this, tr("Unsupported filesystem"), msg);
        }
    } else {
        m_snapshotListModel->clear();
        m_filesystemInfo.reset();
    }

    if (Backup::instance().config().load(location + "/inverita.conf")) {
        updateLatestLink(location);
        m_controlUI->setEnabledButtons(ControlUI::CreateButton, true);
        m_controlUI->setEnabledButtons(ControlUI::VerifyButton, (m_snapshotListModel->count() > 0));
        m_backupSelectorUI->setEnableConfiguration(true);
        m_snapshotListUI->setEnableReload(true);
    } else {
        m_controlUI->setEnabledButtons(ControlUI::AllButtons, false);
        m_backupSelectorUI->setEnableConfiguration(false);
        m_snapshotListUI->setEnableReload(false);
        m_backupSelectorUI->select(-1);
    }
}


void InveritaWindow::closeCurrentBackup()
{
    Backup &backup = Backup::instance();

    while (backup.close() == Backup::Failed) {
        QString msg = tr("The backup can not be closed:") + "<br>" +
                      backup.origin() + "<br><br>" +
                      backup.errorString() + "<br>";
        if (QMessageBox::critical(this, tr("Backup access error"), msg,
                                  QMessageBox::Ignore | QMessageBox::Retry, QMessageBox::Retry) == QMessageBox::Ignore) {
            break;
        }
    }
}


Backup::Status InveritaWindow::openCurrentBackup(BackupEntry &entry)
{
    Backup::Status status = Backup::Failed;

    Backup &backup = Backup::instance();
    backup.use(entry.origin);
    backup.setLabel(entry.label);

    while (status != Backup::Success) {
        if (entry.encryption == Backup::EncFSEncrypted) {

            QString currentPassword = entry.password;
            PasswordDialog passwordDialog(this);
            passwordDialog.setPassword(currentPassword);
            passwordDialog.setRememberPassword(!currentPassword.isEmpty());
            if (passwordDialog.exec() == QDialog::Rejected) {
                break;
            }

            currentPassword = passwordDialog.password();
            if (passwordDialog.rememberPassword()) {
                entry.password = currentPassword;
            } else {
                entry.password.clear();
            }

            backup.setPassword(currentPassword);
            entry.encryption = Backup::EncFSEncrypted;
        }

        QString msg;
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        backup.setEncryption(entry.encryption ? Backup::EncFSEncrypted : Backup::NotEncrypted);
        status = backup.open();
        QApplication::restoreOverrideCursor();

        if (status == Backup::CouldNotStarted) {
            msg = tr("Encryption software 'EncFS' could not be started:") + "<br><br>" +
                  "'" + backup.errorString() + "'<br><br>" +
                  tr("Please verify, if 'EncFS' is installed properly and try again.");
            if (QMessageBox::critical(this, tr("Encryption software missing"), msg,
                                      QMessageBox::Cancel | QMessageBox::Retry, QMessageBox::Cancel) == QMessageBox::Cancel) {
                break;
            }
        }

        if (status == Backup::Failed) {
            msg = tr("The backup can not be accessed:") + "<br>" +
                  backup.origin() + "<br><br>" +
                  backup.errorString();
            if (QMessageBox::critical(this, tr("Backup access error"), msg,
                                      QMessageBox::Cancel | QMessageBox::Retry, QMessageBox::Retry) == QMessageBox::Cancel) {
                break;
            }
        }
    }

    return status;
}


void InveritaWindow::onBackupSelected(int selection)
{
    if (selection < 0) {
        return;
    }

    Backup &backup = Backup::instance();
    BackupEntry entry = m_backupListModel->at(selection);

    // Switch backup only if really a new backup is selected
    if (!backup.isOpen() || backup.origin() != entry.origin) {
        closeCurrentBackup();
        if (openCurrentBackup(entry) != Backup::Success) {
            m_backupSelectorUI->select(-1);
        } else {
            m_backupListModel->setEntry(entry);
            if (QFile::exists(backup.location() + "/inverita.conf") == false) {
                onConfigure();
            }
        }
    }

    reload();
}


void InveritaWindow::abortProgress()
{
    qDebug() << "MainWindow::abortProgress() called";

    // abort() can not be called via event loop (connect), because
    // the worker thread blocks its event queue.
    m_backupEngine.abort();
    m_eraseEngine.abort();
    m_validateEngine.abort();
    m_verifyEngine.abort();
}


void InveritaWindow::onDeleteBackup()
{
    int index = m_snapshotListUI->currentSelection();
    QString name = m_snapshotListModel->at(index).name();
    m_eraseEngine.select(Backup::instance().location() + "/" + name);
    emit deleteBackup();
}


void InveritaWindow::onValidateBackup()
{
    int index = m_snapshotListUI->currentSelection();
    QString name = m_snapshotListModel->at(index).name();
    m_validateEngine.select(Backup::instance().location(), name);
    emit validateBackup();
}


/*! Create or update a symbolic link "latest" pointing to the
 *  latest backup
 *
 * \param absolutePath to the base backup directory
 */
void InveritaWindow::updateLatestLink(QString absolutePath)
{
    QString linkPath = absolutePath + "/latest";
    QString linkName;
    if (QFile::exists(linkPath)) {
        linkName = ReadSymbolicLink(linkPath);
    }

    if (m_snapshotListModel->isEmpty()) {
        QFile::remove(linkPath);
    } else {
        QString newName = m_snapshotListModel->last().name();
        if (linkName != newName) {
            QFile::remove(linkPath);
            QFile::link(newName, linkPath);
        }
    }
}


void InveritaWindow::onMenuNewBackup()
{
    Configuration config;
    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Create a new backup"));
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    closeCurrentBackup();

    BackupEntry entry;
    entry.label = configDialog.label();
    entry.origin = configDialog.location();
    entry.encryption = configDialog.encrypt() ? Backup::EncFSEncrypted : Backup::NotEncrypted;
    entry.password.clear();

    if (openCurrentBackup(entry) == Backup::Success) {
        config.save(Backup::instance().location() + "/inverita.conf");
        int index = m_backupListModel->setEntry(entry);
        m_backupSelectorUI->select(index); // causes a currentIndexChanged event
    } else {
        m_backupSelectorUI->select(-1);
    }
}


void InveritaWindow::onMenuOpenBackup()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Open an existing backup"));
    filedialog.setFileMode(QFileDialog::Directory);
    filedialog.setDirectory(QDir::homePath());
    filedialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    bool validStorage = false;

    BackupEntry entry;
    entry.origin = filedialog.selectedFiles()[0];
    entry.encryption = Backup::NotEncrypted;
    entry.password.clear();

    if (QFile::exists(entry.origin + "/.encfs6.xml")) {
        entry.encryption = Backup::EncFSEncrypted;
        validStorage = true;
    }

    if (QFile::exists(entry.origin + "/inverita.conf")) {
        entry.encryption = Backup::NotEncrypted;
        validStorage = true;
    }

    if (!validStorage) {
        QMessageBox::critical(this,
                              tr("Backup not supported"),
                              tr("The backup you want to open is not supported:<br>'%1'<br>"
                                 "Please choose another backup.").arg(entry.origin)
                             );
        return;
    }

    int index = m_backupListModel->setEntry(entry);
    m_backupSelectorUI->select(index); // causes a currentIndexChanged event
}


void InveritaWindow::onConfigure()
{
    Backup &backup = Backup::instance();

    Configuration config;
    config.load(backup.location() + "/inverita.conf");

    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Configuring backup '%1'").arg(backup.origin()));
    configDialog.setLabel(backup.label());
    configDialog.setLocation(backup.origin());
    configDialog.setEncrypt(backup.encryption() == Backup::EncFSEncrypted);
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    BackupEntry entry;
    entry.label = configDialog.label();
    entry.origin = configDialog.location();
    entry.encryption = configDialog.encrypt() ? Backup::EncFSEncrypted : Backup::NotEncrypted;
    entry.password = backup.password();

    QString newOrigin = configDialog.location();
    if (backup.origin() == newOrigin) {
        config.save(backup.location() + "/inverita.conf");
        m_backupListModel->setEntry(entry);
        reload(); // explicitly reload, because there is no backup change
        return;
    }

    closeCurrentBackup();
    if (openCurrentBackup(entry) == Backup::Success) {
        config.save(newOrigin + "/inverita.conf");
        int index = m_backupListModel->setEntry(entry);
        m_backupSelectorUI->select(index); // causes a currentIndexChanged event
    } else {
        m_backupSelectorUI->select(-1);
    }
}


/*! Creates the actions for the application menu
 */
void InveritaWindow::createActions()
{
    m_createBackupAction = new QAction(tr("Create new backup..."), this);
    m_createBackupAction->setStatusTip(tr("Create new backup configuration"));
    m_createBackupAction->setIconVisibleInMenu(true);
    m_createBackupAction->setIcon(QIcon::fromTheme("document-new"));
    m_createBackupAction->setShortcut(QKeySequence::New);
    connect(m_createBackupAction, SIGNAL(triggered()), this, SLOT(onMenuNewBackup()));

    m_openBackupAction = new QAction(tr("Open existing backup..."), this);
    m_openBackupAction->setStatusTip(tr("Open an existing backup configuration"));
    m_openBackupAction->setIconVisibleInMenu(true);
    m_openBackupAction->setIcon(QIcon::fromTheme("document-open"));
    m_openBackupAction->setShortcut(QKeySequence::Open);
    connect(m_openBackupAction, SIGNAL(triggered()), this, SLOT(onMenuOpenBackup()));

    m_quitAction = new QAction(tr("Quit"), this);
    m_quitAction->setStatusTip(tr("Quit application"));
    m_quitAction->setIconVisibleInMenu(true);
    m_quitAction->setIcon(QIcon::fromTheme("exit"));
    m_quitAction->setShortcut(QKeySequence::Quit);
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));

    m_aboutAction = new QAction(tr("&About INVERITA..."), this);
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    m_aboutAction->setIconVisibleInMenu(true);
    m_aboutAction->setIcon(QIcon::fromTheme("help-about"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}


/*! Creates the application menu
 */
void InveritaWindow::createMenus()
{
    m_backupMenu = menuBar()->addMenu(tr("&Backup"));
    m_backupMenu->addAction(m_createBackupAction);
    m_backupMenu->addAction(m_openBackupAction);
    m_backupMenu->addSeparator();
    m_backupMenu->addAction(m_quitAction);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAction);
}

