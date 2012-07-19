/*
 * mainwindow.cpp
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


#include "mainwindow.h"
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


/*! Constructs a new mainwindow objects and initializes the user interface.
 *
 * \param parent the parent UI element
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    m_filesystemInfo = new FilesystemInfo();

    m_backupListModel = new BackupListModel(this);
    m_backupListModel->load("inverita");
    m_backupSelectorUI = new BackupSelectorUI(m_backupListModel, this);

    m_snapshotListModel = new SnapshotListModel(this);
    m_snapshotListUI = new SnapshotListUI(m_snapshotListModel, this);

    m_driveCapacityUI = new DriveCapacityUI(m_filesystemInfo, this);
    m_controlUI = new ControlUI(this);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_driveCapacityUI, 0, Qt::AlignLeft);
    hlayout->addWidget(m_controlUI, 1, Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(20);
    layout->addWidget(m_backupSelectorUI);
    layout->addWidget(m_snapshotListUI);
    layout->addLayout(hlayout);
    widget->setLayout(layout);

    createActions();
    createMenus();

    m_timer = new QTimer(this);
    m_timer->setInterval(5000);  // for filesystem capacity update

    // The Worker engine should run in its own tread to not
    // block the main event queue.
    m_backupEngine = new BackupEngine();
    m_backupThread = new QThread;
    m_backupEngine->moveToThread(m_backupThread);
    m_backupThread->start(QThread::LowestPriority);

    m_eraseEngine = new EraseEngine();
    m_eraseThread = new QThread;
    m_eraseEngine->moveToThread(m_eraseThread);
    m_eraseThread->start(QThread::LowestPriority);

    m_validateEngine = new ValidateEngine();
    m_validateThread = new QThread;
    m_validateEngine->moveToThread(m_validateThread);
    m_validateThread->start(QThread::LowestPriority);

    m_verifyEngine = new VerifyEngine();
    m_verifyThread = new QThread;
    m_verifyEngine->moveToThread(m_verifyThread);
    m_verifyThread->start(QThread::LowestPriority);

    m_progressBackupDialog = new ProgressDialog(m_backupEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressBackupDialog->setWindowTitle(tr("Creating new backup snapshot..."));
    m_progressEraseDialog = new ProgressDialog(m_eraseEngine, ProgressDialog::NoTextBox, ProgressDialog::NotAbortable, this);
    m_progressEraseDialog->setWindowTitle(tr("Deleting backup snapshot..."));
    m_progressValidateDialog = new ProgressDialog(m_validateEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressValidateDialog->setWindowTitle(tr("Validating backup snapshot..."));
    m_progressVerifyDialog = new ProgressDialog(m_verifyEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressVerifyDialog->setWindowTitle(tr("Verifying latest backup..."));

    connect(m_backupSelectorUI, SIGNAL(backupSelected(int)), this, SLOT(onBackupSelected(int)));
    connect(m_backupSelectorUI, SIGNAL(configure()), this, SLOT(onConfigure()));
    connect(m_snapshotListUI, SIGNAL(reload()), this, SLOT(reload()));
    connect(m_snapshotListUI, SIGNAL(deleteBackup()), this, SLOT(onDeleteBackup()));
    connect(m_snapshotListUI, SIGNAL(validateBackup()), this, SLOT(onValidateBackup()));

    connect(m_controlUI, SIGNAL(startBackup()), m_backupEngine, SLOT(start()));
    connect(m_progressBackupDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(m_backupEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(m_backupEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(m_backupEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));
    connect(m_backupEngine, SIGNAL(report(QString)), m_progressBackupDialog, SLOT(display(QString)));

    connect(this, SIGNAL(deleteBackup()), m_eraseEngine, SLOT(start()));
    connect(m_progressEraseDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(m_eraseEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(m_eraseEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(m_eraseEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));

    connect(this, SIGNAL(validateBackup()), m_validateEngine, SLOT(start()));
    connect(m_progressValidateDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(m_validateEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(m_validateEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(m_validateEngine, SIGNAL(failed()), this, SLOT(onValidationFailed()));
    connect(m_validateEngine, SIGNAL(report(QString)), m_progressValidateDialog, SLOT(display(QString)));

    connect(m_controlUI, SIGNAL(startVerify()), m_verifyEngine, SLOT(start()));
    connect(m_progressVerifyDialog, SIGNAL(aborted()), this, SLOT(abortProgress()));
    connect(m_verifyEngine, SIGNAL(finished()), this, SLOT(reload()));
    connect(m_verifyEngine, SIGNAL(aborted()), this, SLOT(reload()));
    connect(m_verifyEngine, SIGNAL(failed()), this, SLOT(onVerificationFailed()));
    connect(m_verifyEngine, SIGNAL(report(QString)), m_progressVerifyDialog, SLOT(display(QString)));

    connect(m_timer, SIGNAL(timeout()), m_filesystemInfo, SLOT(refresh()));

    m_controlUI->setEnabledButtons(ControlUI::AllButtons, false);
    m_timer->start();

    setWindowTitle(tr("INVERITA Personal Backup"));
    setMinimumSize(750, 600);
    resize(750, 600);

    // center application window on the current screen
    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    move(desktopRect.center() - rect().center());
}


/*! Destructor
 */
MainWindow::~MainWindow()
{

}


void MainWindow::closeEvent(QCloseEvent *event)
{
    closeCurrentBackup();
    QMainWindow::closeEvent(event);
}


/*! Is called if the user selects the "application about" in the menu.
 */
void MainWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}


void MainWindow::onBackupFailed()
{
    QString msg = tr("A critical error during backup execution has occured:\n\n");
    msg += m_backupEngine->failureHint() + "\n";
    msg += tr("Make sure, you have proper privileges to write to the\n"
              "backup medium. If this error occurs for the first time\n"
              "on a long time used backup medium, please consider to\n"
              "replace this medium with a new one.\n\n"

              "Due to backup integrity, this backup snapshot with all\n"
              "contained folders and files should be deleted manually.\n"
              "Please try to solve the problem and restart the backup."
             );

    QMessageBox::critical(this, tr("Backup execution error"), msg);

    m_progressBackupDialog->hide();
    m_progressEraseDialog->hide();
    reload();
}


void MainWindow::onValidationFailed()
{
    m_progressValidateDialog->hide();

    QString msg = tr("A critical error during validation has occured:\n\n");
    msg += m_validateEngine->failureHint() + "\n";
    QMessageBox::critical(this, tr("Snapshot validation error"), msg);

    reload();
}


void MainWindow::onVerificationFailed()
{
    m_progressVerifyDialog->hide();

    QString msg = tr("A critical error during verification has occured:\n\n");
    msg += m_verifyEngine->failureHint() + "\n";
    QMessageBox::critical(this, tr("Backup verification error"), msg);

    reload();
}


void MainWindow::reload()
{
    qDebug() << "MainWindow::reload()";

    QString location = Backup::instance().location();
    if (Backup::instance().isOpen()) {
        m_snapshotListModel->investigate(location);
        m_filesystemInfo->setFile(location);
        if (m_filesystemInfo->filesystemType() != FilesystemInfo::Ext4) {
            QString msg = tr("You are using an unsupported filesystem on your backup medium. "
                             "Currently only 'ext4' filesystems are supported.<br><br>"
                             "<b>You can continue at your own risk!</b>");
            QMessageBox::warning(this, tr("Unsupported filesystem warning"), msg);
        }
    } else {
        m_snapshotListModel->clear();
        m_filesystemInfo->reset();
    }

    if (Backup::instance().config().load(location + "/inverita.conf")) {
        updateLatestLink(location);
        m_controlUI->setEnabledButtons(ControlUI::CreateButton, true);
        m_controlUI->setEnabledButtons(ControlUI::VerifyButton, (m_snapshotListModel->size() > 0));
        m_backupSelectorUI->setEnableConfiguration(true);
        m_snapshotListUI->setEnableReload(true);
    } else {
        m_controlUI->setEnabledButtons(ControlUI::AllButtons, false);
        m_backupSelectorUI->setEnableConfiguration(false);
        m_snapshotListUI->setEnableReload(false);
        m_backupSelectorUI->select(-1);
    }
}


void MainWindow::closeCurrentBackup()
{
    Backup &backup = Backup::instance();

    while (backup.close() == Backup::Failed) {
        QString msg = tr("The backup target can not be closed") + ":\n" +
                      backup.origin() + "\n\n" +
                      backup.errorString() + "\n";
        if (QMessageBox::critical(this, tr("Backup access error"), msg,
                                  QMessageBox::Ignore | QMessageBox::Retry, QMessageBox::Retry) == QMessageBox::Ignore) {
            break;
        }
    }
}


Backup::Status MainWindow::openCurrentBackup(BackupEntry entry)
{
    Backup::Status status = Backup::Failed;

    Backup &backup = Backup::instance();
    backup.use(entry.origin);
    backup.setLabel(entry.label);

    while (status != Backup::Success) {
        if (backup.detectEncryption() != Backup::NotEncrypted) {

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

            entry.encrypted = 1;
            m_backupListModel->setEntry(entry);
        }

        QString msg;
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        status = backup.open();
        QApplication::restoreOverrideCursor();

        if (status == Backup::CouldNotStarted) {
            msg = tr("Encryption software 'encfs' could not be started:\n\n") +
                  "'" + backup.errorString() + "'\n\n" +
                  tr("Please verify, if 'encfs' is installed properly and try again.");
            if (QMessageBox::critical(this, tr("Encryption software missing"), msg,
                                      QMessageBox::Cancel | QMessageBox::Retry, QMessageBox::Cancel) == QMessageBox::Cancel) {
                break;
            }
        }

        if (status == Backup::Failed) {
            msg = tr("The backup can not be accessed") + ":\n" +
                  backup.origin() + "\n\n" +
                  backup.errorString();
            if (QMessageBox::critical(this, tr("Backup access error"), msg,
                                      QMessageBox::Cancel | QMessageBox::Retry, QMessageBox::Retry) == QMessageBox::Cancel) {
                break;
            }
        }
    }

    return status;
}


void MainWindow::onBackupSelected(int selection)
{
    if (selection < 0) {
        return;
    }

    Backup &backup = Backup::instance();
    BackupEntry entry = m_backupListModel->at(selection);

    closeCurrentBackup();
    if (openCurrentBackup(entry) != Backup::Success) {
        m_backupSelectorUI->select(-1);
        return;
    }

    if (QFile::exists(backup.location() + "/inverita.conf") == false) {
        onConfigure();
    }

    reload();
}


void MainWindow::abortProgress()
{
    qDebug() << "MainWindow::abortProgress() called";

    // abort() can not be called via event loop (connect), because
    // the worker thread blocks its event queue.
    m_backupEngine->abort();
    m_eraseEngine->abort();
    m_validateEngine->abort();
    m_verifyEngine->abort();
}


void MainWindow::onDeleteBackup()
{
    int index = m_snapshotListUI->currentSelection();
    QString name = m_snapshotListModel->at(index).name();
    m_eraseEngine->select(Backup::instance().location() + "/" + name);
    emit deleteBackup();
}


void MainWindow::onValidateBackup()
{
    int index = m_snapshotListUI->currentSelection();
    QString name = m_snapshotListModel->at(index).name();
    m_validateEngine->select(Backup::instance().location(), name);
    emit validateBackup();
}


/*! Create or update a symbolic link "latest" pointing to the
 *  latest backup
 *
 * \param absolutePath to the base backup directory
 */
void MainWindow::updateLatestLink(QString absolutePath)
{
    QString linkPath = absolutePath + "/latest";
    QString linkName;
    if (QFile::exists(linkPath)) {
        linkName = ReadLink(linkPath);
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


void MainWindow::onMenuNewBackup()
{
    Configuration config;
    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Create a new backup"));
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    BackupEntry entry;
    entry.label = configDialog.label();
    entry.origin = configDialog.location();
    entry.encrypted = (int)Backup::detectEncryption(entry.origin);
    entry.password.clear();

    Backup &backup = Backup::instance();

    closeCurrentBackup();
    if (openCurrentBackup(entry) == Backup::Success) {
        config.save(backup.location() + "/" + "inverita.conf");
        int index = m_backupListModel->setEntry(entry);
        m_backupSelectorUI->select(index); // causes a currentIndexChanged event
    } else {
        m_backupSelectorUI->select(-1);
    }
}


void MainWindow::onMenuOpenBackup()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Open an existing backup"));
    filedialog.setFileMode(QFileDialog::Directory);
    filedialog.setDirectory(QDir::homePath());
    filedialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    BackupEntry entry;
    entry.origin = filedialog.selectedFiles()[0];
    entry.encrypted = 0;
    entry.password.clear();

    if (Backup::detectEncryption(entry.origin) != Backup::NotEncrypted) {
        entry.encrypted = 1;
    }

    int index = m_backupListModel->setEntry(entry);
    m_backupSelectorUI->select(index); // causes a currentIndexChanged event
}


void MainWindow::onConfigure()
{
    Backup &backup = Backup::instance();

    Configuration config;
    config.load(backup.location() + "/" + "inverita.conf");

    ConfigurationDialog configDialog(config, this);
    configDialog.setWindowTitle(tr("Configuring backup") + "' " + backup.origin() + "'");
    configDialog.setLabel(backup.label());
    configDialog.setLocation(backup.origin());
    if (configDialog.exec() != QDialog::Accepted) {
        return;
    }

    BackupEntry entry;
    entry.label = configDialog.label();
    entry.origin = configDialog.location();
    entry.encrypted = (int)Backup::detectEncryption(entry.origin);
    entry.password = backup.password();

    QString newOrigin = configDialog.location();
    if (backup.origin() == newOrigin) {
        config.save(backup.location() + "/" + "inverita.conf");
        m_backupListModel->setEntry(entry);
        reload(); // explicitly reload, because there is no backup change
        return;
    }

    closeCurrentBackup();
    if (openCurrentBackup(entry) == Backup::Success) {
        config.save(newOrigin + "/" + "inverita.conf");
        int index = m_backupListModel->setEntry(entry);
        m_backupSelectorUI->select(index); // causes a currentIndexChanged event
    } else {
        m_backupSelectorUI->select(-1);
    }
}


/*! Creates the actions for the application menu
 */
void MainWindow::createActions()
{
    createBackupAct = new QAction(tr("Create new backup..."), this);
    createBackupAct->setStatusTip(tr("Create new backup configuration"));
    createBackupAct->setIconVisibleInMenu(true);
    createBackupAct->setIcon(QIcon::fromTheme("document-new"));
    createBackupAct->setShortcut(QKeySequence::New);
    connect(createBackupAct, SIGNAL(triggered()), this, SLOT(onMenuNewBackup()));

    openBackupAct = new QAction(tr("Open existing backup..."), this);
    openBackupAct->setStatusTip(tr("Open an existing backup configuration"));
    openBackupAct->setIconVisibleInMenu(true);
    openBackupAct->setIcon(QIcon::fromTheme("document-open"));
    openBackupAct->setShortcut(QKeySequence::Open);
    connect(openBackupAct, SIGNAL(triggered()), this, SLOT(onMenuOpenBackup()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setStatusTip(tr("Exit application"));
    exitAct->setIconVisibleInMenu(true);
    exitAct->setIcon(QIcon::fromTheme("exit"));
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About..."), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    aboutAct->setIconVisibleInMenu(true);
    aboutAct->setIcon(QIcon::fromTheme("help-about"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt..."), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    aboutQtAct->setIconVisibleInMenu(true);
    aboutQtAct->setIcon(QIcon::fromTheme("help-about"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}


/*! Creates the application menu
 */
void MainWindow::createMenus()
{
    backupMenu = menuBar()->addMenu(tr("&Backup"));
    backupMenu->addAction(createBackupAct);
    backupMenu->addAction(openBackupAct);
    backupMenu->addSeparator();
    backupMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

