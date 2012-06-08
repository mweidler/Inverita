/**
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

BackupEngine::BackupEngine()
{
    reset();

    m_descriptions << tr("1. Analyze backup content") <<
                   tr("2. Create hard link copy") <<
                   tr("3. Generate backup") <<
                   tr("4. Verify generated backup");
}


WorkerStatus BackupEngine::status()
{
    WorkerStatus st;

    qint64 totalFileSize = m_scanTraverser.totalSize();

    qDebug() << "Completion" << m_scanTraverser.totalSize() << m_copyTraverser.totalSize();

    if (totalFileSize == 0) {
        st.completion = 0.0;
    }

    st.completion = ((qreal)m_copyTraverser.totalSize()) / totalFileSize;
    st.transfered = m_copyTraverser.totalSize();
    st.remainingSeconds = 60 * 60;

    return st;
}


// slot, called each time a new backup has selected
void BackupEngine::select(const QString &backupPath)
{
    m_backupRootPath = backupPath;
    qDebug() << "Backup selected: " << m_backupRootPath;
}


// slot called each time, the "Start Backup" button is pressed
// Attention: this will run in a different thread scope
void BackupEngine::start()
{
    reset();
    m_scanTraverser.reset();
    m_copyTraverser.reset();
    emit started();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MMM-dd-hh-mm-ss");
    qDebug() << "startBackup entered: " << m_backupRootPath;

    try {
        m_config.reset();
        m_config.Load(m_backupRootPath + "/inverita.conf");

        m_currentTask = 0;
        scanDirectories();
        m_currentTask = 1;
        executeBackup(timestamp);
        m_currentTask = 2;

        // TODO: if config enabled VerifyBackup();
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


// abort() can not be called via event loop (connect), because
// the worker thread blocks its event queue.
void BackupEngine::abort()
{
    qDebug() << "BackupEngine: abort requested";
    m_abort = true;
    m_scanTraverser.abort();
    m_copyTraverser.abort();
}

void BackupEngine::scanDirectories()
{
    m_scanTraverser.addIncludes(m_config.GetIncludes());
    m_scanTraverser.addExcludes(m_config.GetExcludes());
    m_scanTraverser.traverse();
}

void BackupEngine::executeBackup(QString &timestamp)
{
    QString previousBackup = SearchLatestBackupDir(m_backupRootPath);
    QString currentBackup = m_backupRootPath + "/" + timestamp;

    QDir dir;
    dir.mkpath(currentBackup);

    m_copyTraverser.addIncludes(m_config.GetIncludes());
    m_copyTraverser.addExcludes(m_config.GetExcludes());
    m_copyTraverser.setPreviousBackupPath(previousBackup);
    m_copyTraverser.setCurrentBackupPath(currentBackup);
    m_copyTraverser.previousSignatures().Load(previousBackup + "/" + "signatures");
    m_copyTraverser.traverse();
    m_copyTraverser.currentSignatures().Save(currentBackup + "/" + "signatures");

    SnapshotMetaInfo meta;
    meta.setNumberOfFiles(m_copyTraverser.totalFiles());
    meta.setSizeOfFiles(m_copyTraverser.totalSize());
    meta.Save(currentBackup + "/" + "metainfo");
}
