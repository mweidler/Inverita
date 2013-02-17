/*
 * InveritaWindow.h
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012 - 2013  Marc Weidler <marc.weidler@web.de>,
 *                            Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                            All rights reserved.
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

#ifndef HEADER_INVERITAWINDOW_INC
#define HEADER_INVERITAWINDOW_INC

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
#include "Backup.h"

#include <QThread>


/*! The Main Window
 */
class InveritaWindow : public QMainWindow
{
    Q_OBJECT

public:
    InveritaWindow(QWidget *parent = 0);
    ~InveritaWindow();

signals:
    void deleteBackupSnapshot();
    void validateBackup();
    void verifyBackup();

public slots:
    void about();
    void refreshContent();
    void onBackupSelected(int selection);
    void onOpenSnapshot();
    void onValidateSnapshot();
    void onDeleteSnapshot();
    void onBackupFailed();
    void onValidationFailed();
    void onVerificationFailed();

    void onMenuNewBackup();
    void onMenuOpenBackup();
    void onMenuEmptyBackupList();
    void onConfigure();

protected:
    void closeEvent(QCloseEvent *event);

    void closeCurrentBackup();
    Backup::Status openCurrentBackup(BackupEntry &entry);

protected slots:
    void abortProgress();

private:
    void createActions();
    void createMenus();
    void updateLatestLink(QString absolutePath);

    QMenu   *m_backupMenu;
    QMenu   *m_helpMenu;

    QAction *m_createBackupAction;
    QAction *m_openBackupAction;
    QAction *m_emptyBackupListAction;
    QAction *m_quitAction;
    QAction *m_aboutAction;

    ProgressDialog    *m_progressBackupDialog;
    ProgressDialog    *m_progressEraseDialog;
    ProgressDialog    *m_progressValidateDialog;
    ProgressDialog    *m_progressVerifyDialog;

    BackupSelectorUI  *m_backupSelectorUI;
    SnapshotListUI    *m_snapshotListUI;
    DriveCapacityUI   *m_driveCapacityUI;
    ControlUI         *m_controlUI;

    BackupListModel   *m_backupListModel;
    SnapshotListModel *m_snapshotListModel;

    QTimer            m_timer;
    FilesystemInfo    m_filesystemInfo;
    BackupEngine      m_backupEngine;
    QThread           m_backupThread;
    EraseEngine       m_eraseEngine;
    QThread           m_eraseThread;
    ValidateEngine    m_validateEngine;
    QThread           m_validateThread;
    VerifyEngine      m_verifyEngine;
    QThread           m_verifyThread;
};

#endif // HEADER_INVERITAWINDOW_INC
