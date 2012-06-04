/*
 * mainwindow.cpp
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

    m_backupListModel = new BackupListModel(parent);
    m_backupListModel->Load("inverita");
    m_backupSelectorUI = new BackupSelectorUI(m_backupListModel, this);

    m_historyList = new BackupHistoryList(parent);
    m_backupHistoryUI = new BackupHistoryUI(m_historyList, this);

    m_controlUI = new ControlUI(parent);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(10);
    layout->setSpacing(20);
    layout->addWidget(m_backupSelectorUI);
    layout->addWidget(m_backupHistoryUI);
    layout->addWidget(m_controlUI);
    widget->setLayout(layout);

    createActions();
    createMenus();

    // The Worker engine should run in its own tread to not
    // block the main event queue.
    m_backupEngine = new BackupEngine();
    m_backupThread = new QThread;
    m_backupEngine->moveToThread(m_backupThread);
    m_backupThread->start();

    m_eraseEngine = new EraseEngine();
    m_eraseThread = new QThread;
    m_eraseEngine->moveToThread(m_eraseThread);
    m_eraseThread->start();

    m_progressBackupDialog = new ProgressDialogUI(m_backupEngine, this);
    m_progressEraseDialog = new ProgressDialogUI(m_eraseEngine, this);

    connect(m_backupSelectorUI, SIGNAL(backupSelected()), this, SLOT(onBackupSelected()));
    connect(m_backupHistoryUI, SIGNAL(reload()), this, SLOT(onBackupSelected()));
    connect(m_backupHistoryUI, SIGNAL(deleteBackup()), this, SLOT(onDeleteBackup()));

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

    statusBar()->showMessage(tr("Welcome."));

    setWindowTitle(tr("INVERITA Personal Backup"));
    setMinimumSize(640, 480);
    resize(800, 600);

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

    QString msg = tr(
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
               "Please try to solve the problem and restart the backup.");

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
    m_backupEngine->select(origin);
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

void MainWindow::onDeleteBackup()
{
    int index = m_backupSelectorUI->currentSelection();
    QString origin = m_backupListModel->backupList().at(index).origin;

    index = m_backupHistoryUI->currentSelection();
    QString name = m_historyList->at(index).name;
    m_eraseEngine->select(origin + "/" + name);
    emit deleteBackup();
}

/*! Create or update a symbolic link "latest" pointing to the
 *  latest backup
 *
 * \param absolutePath to the base backup directory
 */
void MainWindow::updateLatestLink(QString &absolutePath)
{
    qDebug() << "absolutePath" << absolutePath;

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
