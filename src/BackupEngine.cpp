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
#include "Configuration.h"
#include "FilesystemInfo.h"
#include "SnapshotListModel.h"
#include "Backup.h"

#include <QDir>
#include <QDebug>


/*! Constructs a new backup engine object.
 */
BackupEngine::BackupEngine()
{
    reset();

    m_descriptions << tr("1. Checking available drive space") <<
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
    qint64 expected = qMax(m_scanTraverser.processed(), (qint64)1);
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
        executeBackup(timestamp);

        if (Backup::instance().config().verifyAfterBackup()) {
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
    m_eraseTraverser.abort();
    m_scanTraverser.abort();
    m_copyTraverser.abort();
    m_validateTraverser.abort();
}


void BackupEngine::checkDriveSpace()
{
    if (!Backup::instance().config().autoDeleteBackups()) {
        return;
    }

    FilesystemInfo filesystem(Backup::instance().location());
    SnapshotListModel snapshotList;
    snapshotList.investigate(Backup::instance().location());
    qreal spare = Backup::instance().config().spareCapacity() / 100.0;

    for (int i = 0; (i < snapshotList.count()) && (filesystem.capacity() < spare); i++) {
        deleteSnapshot(Backup::instance().location() + "/" + snapshotList[i].name());
        emit report(tr("Old backup snapshot '%1' has been deleted due to low drive space.").arg(snapshotList[i].name()));
        filesystem.refresh();
    }
}


void BackupEngine::checkOvercharge()
{
    if (!Backup::instance().config().limitBackups()) {
        return;
    }

    SnapshotListModel snapshotList;
    snapshotList.investigate(Backup::instance().location());
    int overcharge = snapshotList.count() - Backup::instance().config().maximumBackups() ;

    for (int i = 0; i < overcharge && !m_abort; i++) {
        deleteSnapshot(Backup::instance().location() + "/" + snapshotList[i].name());
        emit report(tr("Old backup snapshot '%1' has been deleted due to overcharge.").arg(snapshotList[i].name()));
    }
}


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
void BackupEngine::executeBackup(QString &timestamp)
{
    if (m_abort) {
        return;
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
    m_copyTraverser.traverse();
    QByteArray checksum = m_copyTraverser.currentDigests().save(currentBackup + "/digests");

    SnapshotMetaInfo metaInfo;
    metaInfo.setNumberOfFiles(m_copyTraverser.files());
    metaInfo.setSizeOfFiles(m_copyTraverser.processed());
    metaInfo.setQuality(SnapshotMetaInfo::Partial);

    // set valid flag, if expected data/files could be copied, no errors
    // occured and the backup was not aborted.
    if (m_copyTraverser.files() == m_scanTraverser.files() &&
        m_copyTraverser.processed() == m_scanTraverser.processed() &&
        m_copyTraverser.errors() == 0 &&
        m_abort == false) {
        metaInfo.setQuality(SnapshotMetaInfo::Complete);
        metaInfo.setChecksum(checksum);
        emit report(tr("Backup snapshot created successfully without errors.") + "<br>");
        emit report(tr("Backup snapshot checksum is '%1'").arg(QString(checksum)) + "<br>");
    }
    metaInfo.save(currentBackup + "/metainfo");
}


/*! Sub job of the backup engine: Validates, if the backup is fully readable and
 *  compares content against digests.
 *
 * \param timestamp of the new backup snapshot
 */
void BackupEngine::validateBackup(QString &timestamp)
{
    if (m_abort) {
        return;
    }

    QString snapshotName = Backup::instance().location() + "/@" + timestamp;

    m_validateTraverser.addIncludes(snapshotName);
    m_validateTraverser.addExcludes("metainfo");
    m_validateTraverser.addExcludes("digests");
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
