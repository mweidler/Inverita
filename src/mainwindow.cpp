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
    m_backupListModel->Load("inverita");
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

    // The Worker engine should run in its own tread to not
    // block the main event queue.
    m_backupEngine = new BackupEngine();
    m_backupThread = new QThread;
    m_backupEngine->moveToThread(m_backupThread);
    m_backupThread->start(QThread::IdlePriority);

    m_eraseEngine = new EraseEngine();
    m_eraseThread = new QThread;
    m_eraseEngine->moveToThread(m_eraseThread);
    m_eraseThread->start(QThread::IdlePriority);

    m_validateEngine = new ValidateEngine();
    m_validateThread = new QThread;
    m_validateEngine->moveToThread(m_validateThread);
    m_validateThread->start(QThread::IdlePriority);

    m_verifyEngine = new VerifyEngine();
    m_verifyThread = new QThread;
    m_verifyEngine->moveToThread(m_verifyThread);
    m_verifyThread->start(QThread::IdlePriority);

    m_driveCapacityWatcher = new DriveCapacityWatcher(m_filesystemInfo, m_snapshotListModel, &m_config);
    m_driveWatchThread = new QThread;
    m_driveCapacityWatcher->moveToThread(m_verifyThread);
    m_driveWatchThread->start(QThread::LowPriority); // must have higher priority than backup execution

    m_progressBackupDialog = new ProgressDialog(m_backupEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressBackupDialog->setWindowTitle(tr("Creating new backup snapshot..."));
    m_progressEraseDialog = new ProgressDialog(m_eraseEngine, ProgressDialog::NoTextBox, ProgressDialog::NotAbortable, this);
    m_progressEraseDialog->setWindowTitle(tr("Deleting backup snapshot..."));
    m_progressValidateDialog = new ProgressDialog(m_validateEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressValidateDialog->setWindowTitle(tr("Validating backup snapshot..."));
    m_progressVerifyDialog = new ProgressDialog(m_verifyEngine, ProgressDialog::ShowTextBox, ProgressDialog::Abortable, this);
    m_progressVerifyDialog->setWindowTitle(tr("Verifying latest backup..."));

    connect(m_backupSelectorUI, SIGNAL(backupSelected()), this, SLOT(onBackupSelected()));
    connect(m_snapshotListUI, SIGNAL(reload()), this, SLOT(reload()));
    connect(m_snapshotListUI, SIGNAL(deleteBackup()), this, SLOT(onDeleteBackup()));
    connect(m_snapshotListUI, SIGNAL(validateBackup()), this, SLOT(onValidateBackup()));

    connect(m_controlUI, SIGNAL(startBackup()), m_backupEngine, SLOT(start()));
    connect(m_controlUI, SIGNAL(startBackup()), this, SLOT(onStartBackup()));
    connect(m_progressBackupDialog, SIGNAL(aborted()), this, SLOT(cancelProgress()));
    connect(m_backupEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_backupEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_backupEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));
    connect(m_backupEngine, SIGNAL(report(QString)), m_progressBackupDialog, SLOT(display(QString)));

    connect(this, SIGNAL(deleteBackup()), m_eraseEngine, SLOT(start()));
    connect(m_progressEraseDialog, SIGNAL(aborted()), this, SLOT(cancelProgress()));
    connect(m_eraseEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_eraseEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_eraseEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));

    connect(this, SIGNAL(validateBackup()), m_validateEngine, SLOT(start()));
    connect(m_progressValidateDialog, SIGNAL(aborted()), this, SLOT(cancelProgress()));
    connect(m_validateEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_validateEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_validateEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));
    connect(m_validateEngine, SIGNAL(report(QString)), m_progressValidateDialog, SLOT(display(QString)));

    connect(m_controlUI, SIGNAL(startVerify()), m_verifyEngine, SLOT(start()));
    connect(m_progressVerifyDialog, SIGNAL(aborted()), this, SLOT(cancelProgress()));
    connect(m_verifyEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_verifyEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_verifyEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));
    connect(m_verifyEngine, SIGNAL(report(QString)), m_progressVerifyDialog, SLOT(display(QString)));

    connect(m_driveCapacityWatcher, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_driveCapacityWatcher, SIGNAL(failed()), this, SLOT(onBackupFailed()));

    m_controlUI->setEnabledButtons(ControlUI::AllButtons, false);

    QIcon appicon(QPixmap(":/images/backup-icon.png"));
    setWindowIcon(appicon);
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
    m_backupSelectorUI->unmountEncfs();
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
    qDebug() << "onBackupFailed";
    QString failure = m_backupEngine->failureHint();

    QString msg =
        tr(
            "A critical error during backup execution has occured:\n\n"
        );

    msg += failure;
    msg += "\n";

    msg += tr(
               "Make sure, you have proper privileges to write to the\n"
               "backup medium. If this error occurs for the first time\n"
               "on a long time used backup medium, please consider to\n"
               "replace this medium with a new one.\n\n"

               "Due to backup integrity, this backup snapshot with all\n"
               "contained folders and files should be deleted manually.\n"
               "Please try to solve the problem and restart the backup."
           );

    QMessageBox::critical(0, tr("INVERITA execution error"), msg);

    m_progressBackupDialog->hide();
    m_progressEraseDialog->hide();
    reload();
}


void MainWindow::reload()
{
    qDebug() << "MainWindow::reload()";

    m_snapshotListModel->investigate(m_backupLocation);
    m_filesystemInfo->setFile(m_backupLocation);
    m_backupEngine->select(m_backupLocation);
    m_verifyEngine->select(m_backupLocation);
    m_driveCapacityWatcher->select(m_backupLocation);

    if (m_config.Load(m_backupLocation + "/inverita.conf")) {
        updateLatestLink(m_backupLocation);
        m_controlUI->setEnabledButtons(ControlUI::CreateButton, true);
        m_controlUI->setEnabledButtons(ControlUI::VerifyButton, (m_snapshotListModel->size() > 0));
    } else {
        m_controlUI->setEnabledButtons(ControlUI::AllButtons, false);
    }
}


void MainWindow::onBackupSelected()
{
    qDebug() << "MainWindow::onBackupSelected()" << QThread::currentThreadId();

    int idx = m_backupSelectorUI->currentSelection();
    if (idx < 0) {
        return;
    }

    BackupEntry entry = m_backupListModel->at(idx);
    m_backupLocation = entry.location;
    qDebug() << "Selected" << m_backupLocation;
    reload();
}

void MainWindow::onStartBackup()
{
    qDebug() << "onStartBackup";
    m_driveCapacityWatcher->setAutoDeleteEnabled(true);
}


void MainWindow::onBackupFinished()
{
    qDebug() << "onBackupFinished";
    m_driveCapacityWatcher->setAutoDeleteEnabled(false);
    reload();
}


void MainWindow::onBackupAborted()
{
    qDebug() << "onBackupAborted";
    m_driveCapacityWatcher->setAutoDeleteEnabled(false);
}


void MainWindow::cancelProgress()
{
    qDebug() << "CancelProgress called";

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
    m_eraseEngine->select(m_backupLocation + "/" + name);
    emit deleteBackup();
}


void MainWindow::onValidateBackup()
{
    int index = m_snapshotListUI->currentSelection();
    QString name = m_snapshotListModel->at(index).name();
    m_validateEngine->select(m_backupLocation + "/" + name);
    emit validateBackup();
}


/*! Create or update a symbolic link "latest" pointing to the
 *  latest backup
 *
 * \param absolutePath to the base backup directory
 */
void MainWindow::updateLatestLink(QString &absolutePath)
{
    qDebug() << "updateLatestLink" << absolutePath;

    QString linkPath = absolutePath + "/latest";
    QString linkName;
    if (QFile::exists(linkPath)) {
        linkName = ReadLink(linkPath);
    }

    if (m_snapshotListModel->isEmpty()) {
        QFile::remove(linkPath);
        qDebug() << "latest link removed";
    } else {
        QString newName = m_snapshotListModel->last().name();
        if (linkName != newName) {
            QFile::remove(linkPath);
            QFile::link(newName, linkPath);
            qDebug() << "latest link set to " << newName;
        }
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
    createBackupAct->setShortcut(Qt::ALT | Qt::Key_N);
    connect(createBackupAct, SIGNAL(triggered()), m_backupSelectorUI, SLOT(onNew()));

    selectBackupAct = new QAction(tr("Select existing backup..."), this);
    selectBackupAct->setStatusTip(tr("Select an existing backup configuration"));
    selectBackupAct->setIconVisibleInMenu(true);
    selectBackupAct->setIcon(QIcon::fromTheme("document-open"));
    selectBackupAct->setShortcut(Qt::ALT | Qt::Key_S);
    connect(selectBackupAct, SIGNAL(triggered()), m_backupSelectorUI, SLOT(onSelect()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setStatusTip(tr("Exit application"));
    exitAct->setIconVisibleInMenu(true);
    exitAct->setIcon(QIcon::fromTheme("exit"));
    exitAct->setShortcut(Qt::ALT | Qt::Key_F4);
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
    backupMenu->addAction(selectBackupAct);
    backupMenu->addSeparator();
    backupMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}
