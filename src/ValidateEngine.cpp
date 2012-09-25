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
    m_currentTask = 0;

    // traverser and engine can emit report signals to the progress dialog
    connect(&m_validateTraverser, SIGNAL(report(QString)), this, SIGNAL(report(QString)));
}


/*! \return the number of tasks
 */
int ValidateEngine::taskCount() const
{
    return 1;
}


/*! \return the index of the current task
 */
int ValidateEngine::currentTask() const
{
    return m_currentTask;
}


/*! \return the data stored under the given role for the task referred to by the index
 */
QVariant ValidateEngine::taskData(int /*task*/, int role) const
{
    if (role == Qt::DisplayRole) {
        return QVariant(tr("Validating the selected backup snapshot"));
    }

    if (role == Qt::CheckStateRole) {
        return QVariant(true);
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
WorkerStatus ValidateEngine::status() const
{
    WorkerStatus st;
    st.timestamp  = QDateTime::currentDateTime();
    st.completion = ((qreal)m_validateTraverser.processed()) / qMax(m_metaInfo.dataSize(), (qint64)1);
    st.transferred = m_validateTraverser.transferred();

    return st;
}


void ValidateEngine::select(const QString &backupRootPath, const QString &snapshotName)
{
    m_backupRootPath = backupRootPath;
    m_snapshotName = snapshotName;
}


// slot called each time, the "Start Backup" button is pressed
// Attention: this will run in a different thread scope
void ValidateEngine::start()
{
    reset();
    m_currentTask = 0;

    Configuration config;
    config.load(m_backupRootPath + "/inverita.conf");
    m_validateTraverser.reset();
    QString snapshotName = m_backupRootPath + "/" + m_snapshotName;
    emit started();

    m_metaInfo.load(snapshotName + "/metainfo") ;
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

            m_metaInfo.save(snapshotName + "/metainfo");
            emit finished();

        } catch (ApplicationException &e) {
            m_metaInfo.setQuality(SnapshotMetaInfo::Unknown);
            m_metaInfo.save(snapshotName + "/metainfo");
            buildFailureHint(e);
            emit failed();
        }
    } else {
        m_failureHint = tr("The content of the digests file are not trustable because "
                           "it's checksum does not match the expected checksum!<br>"
                           "A snapshot validation is not possible!<br>"
                           "The backup snapshot status will be set to 'Unknown'.");
        m_metaInfo.setQuality(SnapshotMetaInfo::Unknown);
        m_metaInfo.save(snapshotName + "/metainfo");
        emit failed();
    }

    m_validateTraverser.digests().clear();
}


// abort() can not be called via event loop (connect), because
// the thread blocks its event queue.
void ValidateEngine::abort()
{
    qDebug() << "ValidateEngine: abort requested";
    m_abort = true;
    m_validateTraverser.abort();
}
