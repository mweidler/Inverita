/*
 * mainwindow.h
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

#ifndef HEADER_MAINWINDOW_INC
#define HEADER_MAINWINDOW_INC

#include <QMainWindow>

#include "BackupSelectorUI.h"
#include "SnapshotListUI.h"
#include "DriveCapacityUI.h"
#include "ControlUI.h"
#include "BackupListModel.h"
#include "SnapshotListModel.h"
#include "FilesystemInfo.h"
#include "BackupEngine.h"
#include "EraseEngine.h"
#include "ValidateEngine.h"
#include "VerifyEngine.h"
#include "ProgressDialog.h"
#include "DriveCapacityWatcher.h"
#include "Backup.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void deleteBackup();
    void validateBackup();
    void verifyBackup();

public slots:
    void about();
    void onBackupSelected(int selection);
    void onStartBackup();
    void onBackupFinished();
    void onBackupAborted();
    void onDeleteBackup();
    void onValidateBackup();
    void onBackupFailed();

    void onMenuNewBackup();
    void onMenuSelectBackup();
    void onConfigure();

protected:
    void closeEvent(QCloseEvent *event);

    void closeCurrentBackup();
    Backup::Status openCurrentBackup(BackupEntry entry);

protected slots:
    void abortProgress();
    void reload();

private:
    void createActions();
    void createMenus();
    void updateLatestLink(QString absolutePath);

    QMenu   *backupMenu;
    QMenu   *helpMenu;
    QAction *createBackupAct;
    QAction *selectBackupAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    BackupListModel   *m_backupListModel;
    BackupSelectorUI  *m_backupSelectorUI;
    SnapshotListModel *m_snapshotListModel;
    SnapshotListUI    *m_snapshotListUI;

    FilesystemInfo       *m_filesystemInfo;
    DriveCapacityUI      *m_driveCapacityUI;
    ControlUI            *m_controlUI;
    BackupEngine         *m_backupEngine;
    QThread              *m_backupThread;
    EraseEngine          *m_eraseEngine;
    QThread              *m_eraseThread;
    ValidateEngine       *m_validateEngine;
    QThread              *m_validateThread;
    VerifyEngine         *m_verifyEngine;
    QThread              *m_verifyThread;
    DriveCapacityWatcher *m_driveCapacityWatcher;
    QThread              *m_driveWatchThread;
    ProgressDialog       *m_progressBackupDialog;
    ProgressDialog       *m_progressEraseDialog;
    ProgressDialog       *m_progressValidateDialog;
    ProgressDialog       *m_progressVerifyDialog;
    Configuration        m_config;
    Backup               m_currentBackup;
};

#endif // HEADER_MAINWINDOW_INC
