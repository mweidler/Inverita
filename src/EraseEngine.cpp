/**
 * EraseEngine.cpp
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
// the thread blocks its event queue.
void EraseEngine::abort()
{
    qDebug() << "EraseEngine: abort requested";
    m_abort = true;
    m_eraseTraverser.abort();
}
