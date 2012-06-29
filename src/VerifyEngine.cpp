/*
 * VerifyEngine.cpp
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


#include "VerifyEngine.h"
#include "Utilities.h"

VerifyEngine::VerifyEngine()
{
    reset();

    m_descriptions << tr("Verifying all items of the selected backup");

    // tarverser and engine can emit report signals to the progress dialog
    connect(&m_validateTraverser, SIGNAL(report(QString)), this, SIGNAL(report(QString)));
}


WorkerStatus VerifyEngine::status()
{
    WorkerStatus st;
    st.timestamp  = QDateTime::currentDateTime();
    st.completion = ((qreal)m_validateTraverser.totalSize()) / qMax(m_metaInfo.sizeOfFiles(), (qint64)1);
    st.processed  = m_validateTraverser.totalSize();

    return st;
}


void VerifyEngine::select(const QString &backupPath)
{
    m_backupRootPath = backupPath;
    qDebug() << "Verify selected: " << m_backupRootPath;
}


// slot called each time, the "Start Backup" button is pressed
// Attention: this will run in a different thread scope
void VerifyEngine::start()
{
    reset();
    QString empty;
    QString currentBackup = SearchLatestBackupDir(m_backupRootPath);
    m_config.reset();
    m_config.Load(m_backupRootPath + "/inverita.conf");
    m_validateTraverser.reset();
    emit started();

    try {
        m_metaInfo.Load(currentBackup + "/" + "metainfo") ;
        m_validateTraverser.addIncludes(m_config.GetIncludes());
        m_validateTraverser.addExcludes(m_config.GetExcludes());
        m_validateTraverser.addExcludes("metainfo");
        m_validateTraverser.addExcludes("signatures");
        m_validateTraverser.setBackupPath(empty);
        m_validateTraverser.signatures().Load(currentBackup + "/signatures");        
        m_validateTraverser.traverse();
        m_validateTraverser.summary();
        m_currentTask = -1; // disable highlighted task
    } catch (ApplicationException &e) {
        buildFailureHint(e);
        emit failed();
        return;
    }

    emit finished();
}

// abort() can not be called via event loop (connect), because
// the thread blocks its event queue.
void VerifyEngine::abort()
{
    qDebug() << "VerifyEngine: abort requested";
    m_abort = true;
    m_validateTraverser.abort();
}
