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
#include "BackupListModel.h"
#include "BackupSelectorUI.h"
#include "BackupHistoryList.h"
#include "BackupHistoryUI.h"
#include "ControlUI.h"
#include "BackupEngine.h"
#include "EraseEngine.h"
#include "ValidateEngine.h"
#include "VerifyEngine.h"
#include "FilesystemInfo.h"
#include "DriveCapacityUI.h"
#include "ProgressDialog.h"
#include "DriveCapacityWatcher.h"

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
    void onBackupSelected();
    void onBackupStarted();
    void onBackupFinished();
    void onBackupAborted();
    void onDeleteBackup();
    void onValidateBackup();
    void onBackupFailed();

protected slots:
    void cancelProgress();

private:
    void createActions();
    void createMenus();
    void updateLatestLink(QString &absolutePath);

    QMenu   *backupMenu;
    QMenu   *helpMenu;
    QAction *createBackupAct;
    QAction *chooseBackupAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    BackupListModel   *m_backupListModel;
    BackupSelectorUI  *m_backupSelectorUI;
    BackupHistoryList *m_historyList;
    BackupHistoryUI   *m_backupHistoryUI;

    FilesystemInfo    *m_filesystemInfo;
    DriveCapacityUI   *m_driveCapacityUI;
    ControlUI         *m_controlUI;
    BackupEngine      *m_backupEngine;
    QThread           *m_backupThread;
    EraseEngine       *m_eraseEngine;
    QThread           *m_eraseThread;
    ValidateEngine    *m_validateEngine;
    QThread           *m_validateThread;
    VerifyEngine      *m_verifyEngine;
    QThread           *m_verifyThread;
    DriveCapacityWatcher *m_driveCapacityWatcher;
    QThread           *m_driveWatchThread;
    ProgressDialog    *m_progressBackupDialog;
    ProgressDialog    *m_progressEraseDialog;
    ProgressDialog    *m_progressValidateDialog;
    ProgressDialog    *m_progressVerifyDialog;
};

#endif // HEADER_MAINWINDOW_INC
