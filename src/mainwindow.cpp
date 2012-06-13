/**
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

#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <QtGui>


#include "mainwindow.h"
#include "BackupListModel.h"
#include "BackupSelectorUI.h"
#include "BackupHistoryUI.h"
#include "ControlUI.h"
#include "BackupHistoryList.h"
#include "AboutDialog.h"
#include "Utilities.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    m_filesystemInfo = new FilesystemInfo();

    m_backupListModel = new BackupListModel(parent);
    m_backupListModel->Load("inverita");
    m_backupSelectorUI = new BackupSelectorUI(m_backupListModel, this);

    m_historyList = new BackupHistoryList(parent);
    m_backupHistoryUI = new BackupHistoryUI(m_historyList, this);

    m_driveUsageUI = new DriveUsageUI(m_filesystemInfo, this);
    m_controlUI = new ControlUI(parent);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_driveUsageUI);
    hlayout->addWidget(m_controlUI);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(10);
    layout->setSpacing(20);
    layout->addWidget(m_backupSelectorUI);
    layout->addWidget(m_backupHistoryUI);
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

    m_progressBackupDialog = new ProgressDialogUI(m_backupEngine, ProgressDialogUI::ShowTextBox, this);
    m_progressEraseDialog = new ProgressDialogUI(m_eraseEngine, ProgressDialogUI::NoTextBox, this);
    m_progressValidateDialog = new ProgressDialogUI(m_validateEngine, ProgressDialogUI::ShowTextBox, this);
    m_progressVerifyDialog = new ProgressDialogUI(m_verifyEngine, ProgressDialogUI::ShowTextBox, this);

    connect(m_backupSelectorUI, SIGNAL(backupSelected()), this, SLOT(onBackupSelected()));
    connect(m_backupHistoryUI, SIGNAL(reload()), this, SLOT(onBackupSelected()));
    connect(m_backupHistoryUI, SIGNAL(deleteBackup()), this, SLOT(onDeleteBackup()));
    connect(m_backupHistoryUI, SIGNAL(validateBackup()), this, SLOT(onValidateBackup()));

    connect(m_controlUI, SIGNAL(backupStarted()), m_backupEngine, SLOT(start()));
    connect(m_progressBackupDialog, SIGNAL(aborted()), this, SLOT(cancelBackupProgress()));
    connect(m_backupEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_backupEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_backupEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));

    connect(this, SIGNAL(deleteBackup()), m_eraseEngine, SLOT(start()));
    connect(m_progressEraseDialog, SIGNAL(aborted()), this, SLOT(cancelEraseProgress()));
    connect(m_eraseEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_eraseEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_eraseEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));

    connect(this, SIGNAL(validateBackup()), m_validateEngine, SLOT(start()));
    connect(m_progressValidateDialog, SIGNAL(aborted()), this, SLOT(cancelValidateProgress()));
    connect(m_validateEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_validateEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_validateEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));
    connect(m_validateEngine, SIGNAL(report(QString)), m_progressValidateDialog, SLOT(display(QString)));

    connect(m_controlUI, SIGNAL(startVerify()), m_verifyEngine, SLOT(start()));
    connect(m_progressVerifyDialog, SIGNAL(aborted()), this, SLOT(cancelVerifyProgress()));
    connect(m_verifyEngine, SIGNAL(finished()), this, SLOT(onBackupFinished()));
    connect(m_verifyEngine, SIGNAL(aborted()), this, SLOT(onBackupAborted()));
    connect(m_verifyEngine, SIGNAL(failed()), this, SLOT(onBackupFailed()));
    connect(m_verifyEngine, SIGNAL(report(QString)), m_progressVerifyDialog, SLOT(display(QString)));

    statusBar()->showMessage(tr("Welcome."));

    //setWindowIcon()  TODO
    setWindowTitle(tr("INVERITA Personal Backup"));
    setMinimumSize(800, 700);
    resize(800, 700);

    onBackupSelected();
}

MainWindow::~MainWindow()
{

}

void MainWindow::about()
{
    AboutDialog dialog;
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
    onBackupSelected();
}

void MainWindow::onBackupSelected()
{
    qDebug() << "onBackupSelected";
    int index = m_backupSelectorUI->currentSelection();
    QString origin = m_backupListModel->backupList().at(index).origin;
    m_historyList->investigate(origin);
    m_filesystemInfo->setFile(origin);
    m_backupEngine->select(origin);
    m_verifyEngine->select(origin);
    updateLatestLink(origin);
}

void MainWindow::onBackupFinished()
{
    std::cerr << "onBackupFinished\n";
    onBackupSelected();
}

void MainWindow::onBackupAborted()
{
    std::cerr << "onBackupAborted\n";
}

void MainWindow::cancelBackupProgress()
{
    std::cerr << "CancelBackupProgress called\n";

    // abort() can not be called via event loop (connect), because
    // the worker thread blocks its event queue.
    m_backupEngine->abort();
}

void MainWindow::cancelEraseProgress()
{
    std::cerr << "CancelEraseProgress called\n";

    // abort() can not be called via event loop (connect), because
    // the worker thread blocks its event queue.
    m_eraseEngine->abort();
}

void MainWindow::cancelValidateProgress()
{
    std::cerr << "CancelValidateProgress called\n";

    // abort() can not be called via event loop (connect), because
    // the worker thread blocks its event queue.
    m_validateEngine->abort();
}

void MainWindow::cancelVerifyProgress()
{
    std::cerr << "CancelVerifyProgress called\n";

    // abort() can not be called via event loop (connect), because
    // the worker thread blocks its event queue.
    m_verifyEngine->abort();
}

void MainWindow::onDeleteBackup()
{
    int index = m_backupSelectorUI->currentSelection();
    QString origin = m_backupListModel->backupList().at(index).origin;

    index = m_backupHistoryUI->currentSelection();
    QString name = m_historyList->at(index).name;
    m_eraseEngine->select(origin + "/" + name);
    emit deleteBackup();
}

void MainWindow::onValidateBackup()
{
    int index = m_backupSelectorUI->currentSelection();
    QString origin = m_backupListModel->backupList().at(index).origin;

    index = m_backupHistoryUI->currentSelection();
    QString name = m_historyList->at(index).name;
    m_validateEngine->select(origin + "/" + name);
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

    if (m_historyList->isEmpty()) {
        if (linkName.size() > 0) {
            QFile::remove(linkPath);
            qDebug() << "latest link removed";
        }
    } else {
        QString newName = m_historyList->last().name;
        if (linkName != newName) {
            QFile::remove(linkPath);
            QFile::link(newName, linkPath);
            qDebug() << "latest link set to " << newName;
        }
    }
}

void MainWindow::createActions()
{
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    aboutAct->setIconVisibleInMenu(true);
    aboutAct->setIcon(QIcon::fromTheme("help-about"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    aboutQtAct->setIconVisibleInMenu(true);
    aboutQtAct->setIcon(QIcon::fromTheme("help-about"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}
