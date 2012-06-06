/**
 * ValidateEngine.cpp
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


#include "ValidateEngine.h"
#include "Utilities.h"

ValidateEngine::ValidateEngine()
{
    reset();

    // tarverser and engine can emit notify signals to the progress dialog
    connect(&m_validateTraverser, SIGNAL(notify(QString)), this, SIGNAL(notify(QString)));

    m_descriptions << tr("Validating all items of the selected backup snapshot");
}


qreal ValidateEngine::completion()
{
    qreal expectedSize = m_metaInfo.sizeOfFiles();
    qint64 totalSize = m_validateTraverser.totalSize();

    qDebug() << "Completion" << totalSize << expectedSize;

    if (expectedSize == 0) {
        return 0.0;
    }

    return ((qreal)totalSize) / expectedSize;
}


int ValidateEngine::remainingSeconds()
{
    return 60 * 60 * 2;
}

void ValidateEngine::select(const QString &snapshotName)
{
    m_snapshotName = snapshotName;
    qDebug() << "Validate snapshot selected: " << m_snapshotName;
}


// slot called each time, the "Start Backup" button is pressed
// Attention: this will run in a different thread scope
void ValidateEngine::start()
{
    reset();
    m_validateTraverser.reset();
    emit started();

    qDebug() << "validate entered: " << m_snapshotName;

    try {
        m_metaInfo.Load(m_snapshotName + "/" + "metainfo") ;
        m_validateTraverser.addIncludes(m_snapshotName);
        m_validateTraverser.addExcludes("metainfo");
        m_validateTraverser.addExcludes("signatures");
        m_validateTraverser.setBackupPath(m_snapshotName);
        m_validateTraverser.signatures().Load(m_snapshotName + "/signatures");
        m_validateTraverser.traverse();
    } catch (ApplicationException &e) {
        buildFailureHint(e);
        emit failed();
        return;
    }

    emit finished();
}

// abort() can not be called via event loop (connect), because
// the thread blocks its event queue.
void ValidateEngine::abort()
{
    qDebug() << "ValidateEngine: abort requested";
    m_abort = true;
    m_validateTraverser.abort();
}
