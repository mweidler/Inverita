/*
 * EraseEngine.cpp
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


#include "EraseEngine.h"
#include "Utilities.h"

EraseEngine::EraseEngine()
{
    reset();

    m_descriptions << tr("Deleting all items of the selected backup snapshot");
}

qreal EraseEngine::completion()
{
    // one additional file to delete: "metainfo"
    qreal expectedFiles = m_metaInfo.numberOfFiles() + 1;

    qDebug() << "Completion" << expectedFiles << m_eraseTraverser.totalFiles();

    return m_eraseTraverser.totalFiles() / expectedFiles;
}

int EraseEngine::remainingSeconds()
{
    return 60 * 60 * 2;
}

void EraseEngine::select(const QString &snapshotName)
{
    m_snapshotName = snapshotName;
    qDebug() << "Delete snapshot selected: " << m_snapshotName;
}


// slot called each time, the "Start Backup" button is pressed
// Attention: this will run in a different thread scope
void EraseEngine::start()
{
    reset();
    m_eraseTraverser.reset();
    emit started();

    qDebug() << "deleteBackup entered: " << m_snapshotName;

    QDir rootPath(m_snapshotName);
    rootPath.cdUp();
    QString backupRootPath = rootPath.absolutePath();

    qDebug() << "rootDir: " << backupRootPath;

    try {
        m_metaInfo.Load(m_snapshotName + "/" + "metainfo") ;
        m_eraseTraverser.addIncludes(m_snapshotName);
        m_eraseTraverser.traverse();
    } catch (ApplicationException &e) {
        buildFailureHint(e);
        emit failed();
        return;
    }

    emit finished();
}

// abort() can not be called via event loop (connect), because
// the alvara thread blocks its event queue.
void EraseEngine::abort()
{
    qDebug() << "EraseEngine: abort requested";
    m_abort = true;
    m_eraseTraverser.abort();
}
