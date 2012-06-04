/*
 * BackupEngine.cpp
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


qreal BackupEngine::completion()
{
    qint64 totalFileSize = m_scanTraverser.totalSize();

    qDebug() << "Completion" << m_scanTraverser.totalSize() << m_copyTraverser.totalSize();

    if (totalFileSize == 0) {
        return 0.0;
    }

    return ((qreal)m_copyTraverser.totalSize()) / totalFileSize;
}

int BackupEngine::remainingSeconds()
{
    return 60 * 60 * 2;
}

// slot, called each time a new backup has selected
void BackupEngine::select(const QString &backupPath)
{
    m_backupRootPath = backupPath;
    qDebug() << "Alvara backup selected: " << m_backupRootPath;
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
        m_config.Load(m_backupRootPath + "/alvara.conf");

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
// the alvara thread blocks its event queue.
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
