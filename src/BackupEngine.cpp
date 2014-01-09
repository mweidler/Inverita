/*
 * BackupEngine.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2014  Marc Weidler <marc.weidler@web.de>,
 *                          Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                          All rights reserved.
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
#include "Configuration.h"
#include "FilesystemInfo.h"
#include "SnapshotListModel.h"
#include "Backup.h"

#include <QDir>
#include <unistd.h>


/*! Constructs a new backup engine object.
 */
BackupEngine::BackupEngine()
{
    reset();
    m_currentTask = 0;

    // traverser and engine can emit report signals to the progress dialog
    connect(&m_eraseTraverser,    SIGNAL(report(QString)), this, SIGNAL(report(QString)));
    connect(&m_scanTraverser,     SIGNAL(report(QString)), this, SIGNAL(report(QString)));
    connect(&m_copyTraverser,     SIGNAL(report(QString)), this, SIGNAL(report(QString)));
    connect(&m_validateTraverser, SIGNAL(report(QString)), this, SIGNAL(report(QString)));
}


/*! \return the number of tasks
 */
int BackupEngine::taskCount() const
{
    return 5;
}


/*! \return the index of the current task
 */
int BackupEngine::currentTask() const
{
    return m_currentTask;
}


/*! \return the data stored under the given role for the task referred to by the index
 */
QVariant BackupEngine::taskData(int task, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (task) {
            case 0:
                return QVariant(tr("1. Checking available drive space"));
                break;
            case 1:
                return QVariant(tr("2. Analyzing backup content"));
                break;
            case 2:
                return QVariant(tr("3. Creating new backup snapshot"));
                break;
            case 3:
                return QVariant(tr("4. Verifying generated backup snapshot"));
                break;
            case 4:
                return QVariant(tr("5. Removing obsolete backup snapshots"));
                break;
        }
    }

    if (role == Qt::CheckStateRole) {
        if (task == 3 && Backup::instance().config().verifyAfterBackup() == false) {
            return QVariant(false);
        } else {
            return QVariant(true);
        }
    }

    return QVariant();
}


/*! Returns the current \em WorkerStatus of this engine,
 *  containing \em completion and \em processed bytes.
 *
 *  The worker status is requested by the progress dialog
 *  to present the current job status to the user.
 *
 *  \return the current \em WorkerStatus
 */
WorkerStatus BackupEngine::status() const
{
    qint64 expected = qMax(m_scanTraverser.processed(), Q_INT64_C(1));
    qint64 current = m_copyTraverser.processed() + m_validateTraverser.processed();

    // Completion assumption: copying takes the same time as validation.
    if (Backup::instance().config().verifyAfterBackup()) {
        expected *= 2;
    }

    WorkerStatus st;
    st.timestamp  = QDateTime::currentDateTime();
    st.completion = ((qreal)current) / expected;
    st.transferred = m_copyTraverser.transferred() + m_validateTraverser.transferred();

    return st;
}


/*! This slot is called each time, the "Create Backup" button is pressed
 *  by the user and a new backup shall be created.
 *
 *  \em Attention: this method will run in it's own thread scope.
 */
void BackupEngine::start()
{
    reset();
    m_currentTask = 0;
    m_scanTraverser.reset();
    m_copyTraverser.reset();
    m_validateTraverser.reset();
    m_eraseTraverser.reset();
    emit started();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");

    try {
        m_currentTask = 0;
        checkDriveSpace();
        m_currentTask = 1;
        scanDirectories();
        m_currentTask = 2;
        ExecutionStatus status = executeBackup(timestamp);

        if (status == BackupEngine::Success && Backup::instance().config().verifyAfterBackup()) {
            sync();
            m_currentTask = 3;
            validateBackup(timestamp);
        }

        m_currentTask = 4;
        checkOvercharge();

        m_currentTask = -1; // disable highlighted task

        if (m_abort) {
            emit aborted();
        } else {
            emit finished();
        }

    } catch (ApplicationException &e) {
        buildFailureHint(e);
        emit failed();
    }

    // This is only to reduce memory consumption
    m_copyTraverser.currentDigests().clear();
    m_copyTraverser.previousDigests().clear();
    m_validateTraverser.digests().clear();
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
    m_abort = true;
    m_eraseTraverser.abort();
    m_scanTraverser.abort();
    m_copyTraverser.abort();
    m_validateTraverser.abort();
}


/*! Checks the available drive space and deletes backup snapshots, if the
 *  capacity goes below the configured minimum.
 */
void BackupEngine::checkDriveSpace()
{
    if (!Backup::instance().config().autoDeleteSnapshots()) {
        return;
    }

    FilesystemInfo filesystem(Backup::instance().location());
    SnapshotListModel snapshotList;
    snapshotList.investigate(Backup::instance().location());
    qreal spare = Backup::instance().config().spareCapacity() / 100.0;

    while ((snapshotList.count() > 0) && (filesystem.capacity() < spare)) {
        int idx = snapshotList.count() - 1;
        deleteSnapshot(Backup::instance().location() + "/" + snapshotList[idx].name());
        emit report(tr("Old backup snapshot '%1' has been deleted due to low drive space.").arg(snapshotList[idx].name()) + "<br>");
        snapshotList.investigate(Backup::instance().location());
        filesystem.refresh();
    }
}


/*! Checks the number of backup snapshots and deletes the oldest ones, until the
 *  maximum limit is reached.
 */
void BackupEngine::checkOvercharge()
{
    if (!Backup::instance().config().limitSnapshots()) {
        return;
    }

    SnapshotListModel snapshotList;
    snapshotList.investigate(Backup::instance().location());
    int overcharge = snapshotList.count() - Backup::instance().config().maximumSnapshots() ;
    int idx = snapshotList.count() - 1;

    for (int i = 0; i < overcharge && !m_abort; i++, idx--) {
        deleteSnapshot(Backup::instance().location() + "/" + snapshotList[idx].name());
        emit report(tr("Old backup snapshot '%1' has been deleted due to overcharge.").arg(snapshotList[idx].name()) + "<br>");
    }
}


/*! Delete a snapshot and all of it's files recursively.
 *
 * \param snapshotName the directory name of the snapshot to delete
 */
void BackupEngine::deleteSnapshot(const QString &snapshotName)
{
    m_eraseTraverser.reset();
    m_eraseTraverser.addIncludes(snapshotName);

    // remove metainfo and digests of the snapshot "manually" to
    // invalidate whole snapshot. They were not counted on meta data creation.
    QFile::remove(snapshotName + "/metainfo");
    QFile::remove(snapshotName + "/digests");

    m_eraseTraverser.traverse();
}


/*! Sub job of the backup engine: Scans the requested directory and
 *  counts the number of files and their size using a \em traverser.
 */
void BackupEngine::scanDirectories()
{
    if (m_abort) {
        return;
    }

    m_scanTraverser.addIncludes(Backup::instance().config().includes());
    m_scanTraverser.addExcludes(Backup::instance().config().excludes());
    m_scanTraverser.traverse();

    QString msg = tr("Backup snapshot comprises %1 in %2 files.");
    emit report(msg.arg(formatSize(m_scanTraverser.processed())).arg(m_scanTraverser.files()) + "<br>");
}


/*! Sub job of the backup engine: Copies the files and elements to the
 *  backup directory using a \em traverser.
 *
 * \param timestamp of the new backup snapshot
 */
BackupEngine::ExecutionStatus BackupEngine::executeBackup(const QString &timestamp)
{
    if (m_abort) {
        return BackupEngine::Aborted;
    }

    QString previousBackup = SearchLatestBackupDir(Backup::instance().location());
    QString currentBackup = Backup::instance().location() + "/@" + timestamp;
    QDir dir;

    // ensure, that the new directory of the new snapshot does not exist
    while (QFile::exists(currentBackup)) {
        currentBackup += "x";
    }

    dir.mkpath(currentBackup);

    m_copyTraverser.addIncludes(Backup::instance().config().includes());
    m_copyTraverser.addExcludes(Backup::instance().config().excludes());
    m_copyTraverser.setPreviousBackupPath(previousBackup);
    m_copyTraverser.setCurrentBackupPath(currentBackup);
    m_copyTraverser.previousDigests().load(previousBackup + "/digests");
    m_copyTraverser.currentDigests().clear();
    m_copyTraverser.traverse();
    QByteArray checksum = m_copyTraverser.currentDigests().save(currentBackup + "/digests");

    SnapshotMetaInfo metaInfo;
    metaInfo.setFileCount(m_copyTraverser.files());
    metaInfo.setDataSize(m_copyTraverser.processed());
    metaInfo.setQuality(SnapshotMetaInfo::Partial);
    metaInfo.save(currentBackup + "/metainfo");

    if (m_copyTraverser.errors() > 0) {
        emit report(tr("%n error(s) occured during processing.", "", m_copyTraverser.errors()) + "<br>");
        return BackupEngine::Failed;
    }

    metaInfo.setChecksum(checksum);
    metaInfo.setQuality(SnapshotMetaInfo::Complete);
    metaInfo.save(currentBackup + "/metainfo");
    emit report(tr("Backup snapshot created successfully without errors.") + "<br>");
    emit report(tr("Backup snapshot checksum is '%1'").arg(QString(checksum)) + "<br>");

    return BackupEngine::Success;
}


/*! Sub job of the backup engine: Validates, if the backup is fully readable and
 *  compares content against digests.
 *
 * \param timestamp of the new backup snapshot
 */
void BackupEngine::validateBackup(const QString &timestamp)
{
    if (m_abort) {
        return;
    }

    QString snapshotName = Backup::instance().location() + "/@" + timestamp;

    m_validateTraverser.addIncludes(snapshotName);
    m_validateTraverser.addExcludes(snapshotName + "/metainfo");
    m_validateTraverser.addExcludes(snapshotName + "/digests");
    m_validateTraverser.setBackupPath(snapshotName);
    m_validateTraverser.digests().load(snapshotName + "/digests");
    m_validateTraverser.traverse();
    if (m_abort) {
        return;
    }

    SnapshotMetaInfo metaInfo;
    metaInfo.load(snapshotName + "/metainfo");
    m_validateTraverser.evaluate(metaInfo);
    metaInfo.save(snapshotName + "/metainfo");
}
