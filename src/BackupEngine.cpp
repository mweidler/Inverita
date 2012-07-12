/*
 * BackupEngine.cpp
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


#include "BackupEngine.h"
#include "SnapshotMetaInfo.h"
#include "Utilities.h"
#include "FilesystemInfo.h"
#include "SnapshotListModel.h"

#include <QDebug>


/*! Constructs a new backup engine object.
 */
BackupEngine::BackupEngine()
{
    reset();

    m_descriptions << tr("1. Checking drive space") <<
                   tr("2. Analyzing backup content") <<
                   tr("3. Creating new backup snapshot") <<
                   tr("4. Verifying generated backup snapshot") <<
                   tr("5. Removing obsolete backup snapshots");

    // traverser and engine can emit report signals to the progress dialog
    connect(&m_validateTraverser, SIGNAL(report(QString)), this, SIGNAL(report(QString)));
}


/*! Returns the current \em WorkerStatus of this engine,
 *  containing \em completion and \em processed bytes.
 *
 *  The worker status is requested by the progress dialog
 *  to present the current job status to the user.
 *
 *  \return the current \em WorkerStatus
 */
WorkerStatus BackupEngine::status()
{
    qint64 expected = qMax(m_scanTraverser.totalSize(), (qint64)1);
    qint64 current = m_copyTraverser.totalSize() + m_validateTraverser.totalSize();

    // Completion assumption: copying takes the same time as validation.
    if (m_config.verifyAfterBackup()) {
        expected *= 2;
    }

    WorkerStatus st;
    st.timestamp  = QDateTime::currentDateTime();
    st.completion = ((qreal)current) / expected;
    st.processed  = current;

    return st;
}


/*! Is called each time a new backup has selected (by the user)
 *  to inform the engine of a new backup root path.
 *
 * \param backupPath the new backup root path
 */
void BackupEngine::select(const QString &backupPath)
{
    m_backupRootPath = backupPath;
}


/*! This slot is called each time, the "Create Backup" button is pressed
 *  by the user and a new backup shall be created.
 *
 *  \em Attention: this method will run in it's own thread scope.
 */
void BackupEngine::start()
{
    reset();
    m_scanTraverser.reset();
    m_copyTraverser.reset();
    m_validateTraverser.reset();
    m_eraseTraverser.reset();
    emit started();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");

    try {
        m_config.reset();
        m_config.load(m_backupRootPath + "/inverita.conf");

        m_currentTask = 0;
        checkDriveSpace();
        m_currentTask = 1;
        scanDirectories();
        m_currentTask = 2;
        executeBackup(timestamp);

        if (m_config.verifyAfterBackup()) {
            m_currentTask = 3;
            validateBackup(timestamp);
        }

        m_currentTask = 4;
        checkOvercharge();

        m_currentTask = -1; // disable highlighted task

    } catch (ApplicationException &e) {
        buildFailureHint(e);
        emit failed();
        return;
    }

    if (m_abort) {
        emit aborted();
    } else {
        emit finished();
    }
}


/*! This slot is called each time, the worker job (thread) should be
 *  aborted (self-controlled by the thread). So, this call is only a
 *  \em request to abort and does not terminate the thread immediately.
 *  All currently running tasks should be stopped by the engine
 *  implementation, finally emitting a \em finished-event.
 *
 *  \em Attention: abort() can not be called via event loop (connect),
 *                 because the worker thread blocks its event queue.
 *                 The calling thread will hang!
 */
void BackupEngine::abort()
{
    qDebug() << "BackupEngine: abort requested";
    m_abort = true;
    m_scanTraverser.abort();
    m_copyTraverser.abort();
    m_validateTraverser.abort();
}


void BackupEngine::checkDriveSpace()
{
    if (!m_config.autoDeleteBackups()) {
        return;
    }

    FilesystemInfo filesystem(m_backupRootPath);
    SnapshotListModel snapshotList;
    snapshotList.investigate(m_backupRootPath);
    qreal spare = m_config.spareCapacity() / 100.0;

    for (int i = 0; (i < snapshotList.count()) && (filesystem.capacity() < spare); i++) {
        deleteSnapshot(m_backupRootPath + "/" + snapshotList[i].name());
        filesystem.refresh();
    }
}


void BackupEngine::checkOvercharge()
{
    if (!m_config.limitBackups()) {
        return;
    }

    SnapshotListModel snapshotList;
    snapshotList.investigate(m_backupRootPath);
    int overcharge = snapshotList.count() - m_config.maximumBackups() ;

    for (int i = 0; i < overcharge; i++) {
        deleteSnapshot(m_backupRootPath + "/" + snapshotList[i].name());
        emit report(tr("Backup snapshot deleted due to overcharge: ") + snapshotList[i].name());
    }
}


void BackupEngine::deleteSnapshot(QString snapshotName)
{
    qDebug() << "BackupEngine::deleteSnapshot" << snapshotName;

    m_eraseTraverser.reset();
    m_eraseTraverser.addIncludes(snapshotName);

    // remove metainfo and signatures of the snapshot "manually" to
    // invalidate whole snapshot. They were not counted on meta data creation.
    QFile::remove(snapshotName + "/" + "metainfo");
    QFile::remove(snapshotName + "/" + "signatures");

    m_eraseTraverser.traverse();
}


/*! Sub job of the backup engine: Scans the requested directory and
 *  counts the number of files and their size using a \em traverser.
 */
void BackupEngine::scanDirectories()
{
    m_scanTraverser.addIncludes(m_config.includes());
    m_scanTraverser.addExcludes(m_config.excludes());
    m_scanTraverser.traverse();
}


/*! Sub job of the backup engine: Copies the files and elements to the
 *  backup directory using a \em traverser.
 *
 * \param timestamp of the new backup snapshot
 */
void BackupEngine::executeBackup(QString &timestamp)
{
    QString previousBackup = SearchLatestBackupDir(m_backupRootPath);
    QString currentBackup = m_backupRootPath + "/" + "@" + timestamp;
    QDir dir;

    // ensure, that the new directory of the new snapshot does not exist
    while (QFile::exists(currentBackup)) {
        currentBackup += "1";
    }

    dir.mkpath(currentBackup);

    m_copyTraverser.addIncludes(m_config.includes());
    m_copyTraverser.addExcludes(m_config.excludes());
    m_copyTraverser.setPreviousBackupPath(previousBackup);
    m_copyTraverser.setCurrentBackupPath(currentBackup);
    m_copyTraverser.previousSignatures().load(previousBackup + "/" + "signatures");
    m_copyTraverser.traverse();
    m_copyTraverser.currentSignatures().save(currentBackup + "/" + "signatures");

    m_metaInfo.reset();
    m_metaInfo.setNumberOfFiles(m_copyTraverser.totalFiles());
    m_metaInfo.setSizeOfFiles(m_copyTraverser.totalSize());
    m_metaInfo.save(currentBackup + "/" + "metainfo");
}


/*! Sub job of the backup engine: Validates, if the backup is fully readable and
 *  compares content against hash signatures.
 *
 * \param timestamp of the new backup snapshot
 */
void BackupEngine::validateBackup(QString &timestamp)
{
    QString snapshotName = m_backupRootPath + "/" + "@" + timestamp;

    m_validateTraverser.addIncludes(snapshotName);
    m_validateTraverser.addExcludes("metainfo");
    m_validateTraverser.addExcludes("signatures");
    m_validateTraverser.setBackupPath(snapshotName);
    m_validateTraverser.signatures().load(snapshotName + "/signatures");
    m_validateTraverser.traverse();
    m_validateTraverser.summary();
}
