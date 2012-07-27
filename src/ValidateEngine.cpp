/*
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

#include <QDebug>


ValidateEngine::ValidateEngine()
{
    reset();

    m_descriptions << tr("Validating all items of the selected backup snapshot");

    // traverser and engine can emit report signals to the progress dialog
    connect(&m_validateTraverser, SIGNAL(report(QString)), this, SIGNAL(report(QString)));
}


WorkerStatus ValidateEngine::status()
{
    WorkerStatus st;
    st.timestamp  = QDateTime::currentDateTime();
    st.completion = ((qreal)m_validateTraverser.processed()) / qMax(m_metaInfo.sizeOfFiles(), (qint64)1);
    st.transferred = m_validateTraverser.transferred();

    return st;
}


void ValidateEngine::select(const QString &backupRootPath, const QString &snapshotName)
{
    m_backupRootPath = backupRootPath;
    m_snapshotName = snapshotName;
    qDebug() << "Validate snapshot selected: " << m_snapshotName;
}


// slot called each time, the "Start Backup" button is pressed
// Attention: this will run in a different thread scope
void ValidateEngine::start()
{
    reset();
    Configuration config;
    config.load(m_backupRootPath + "/inverita.conf");
    m_validateTraverser.reset();
    QString snapshotName = m_backupRootPath + "/" + m_snapshotName;
    emit started();

    m_metaInfo.load(snapshotName + "/" + "metainfo") ;
    m_validateTraverser.addIncludes(snapshotName);
    m_validateTraverser.addExcludes("metainfo");
    m_validateTraverser.addExcludes("digests");
    m_validateTraverser.setBackupPath(snapshotName);

    QByteArray checksum = m_validateTraverser.digests().load(snapshotName + "/digests");
    if (checksum == m_metaInfo.checksum()) {
        try {
            m_validateTraverser.traverse();
            m_validateTraverser.evaluate(m_metaInfo);
            m_currentTask = -1; // disable highlighted task
        } catch (ApplicationException &e) {
            m_metaInfo.setQuality(SnapshotMetaInfo::Unknown);
            m_metaInfo.save(snapshotName + "/" + "metainfo");
            buildFailureHint(e);
            emit failed();
            return;
        }
    } else {
        m_failureHint = tr("The contents of digests file are not trustable because it's checksum does not match the expected checksum.<br>"
                           "A snapshot validation is not possible!<br>The snapshot will be set as 'Invalid'.");
        m_metaInfo.setQuality(SnapshotMetaInfo::Unknown);
        m_metaInfo.save(snapshotName + "/" + "metainfo");
        emit failed();
        return;
    }

    m_metaInfo.save(snapshotName + "/" + "metainfo");
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
