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
#include "Backup.h"

#include <QDebug>


/*! Constructor
 */
VerifyEngine::VerifyEngine()
{
    reset();
    m_currentTask = 0;

    // traverser and engine can emit report signals to the progress dialog
    connect(&m_validateTraverser, SIGNAL(report(QString)), this, SIGNAL(report(QString)));
}


/*! \return the number of tasks
 */
int VerifyEngine::taskCount() const
{
    return 1;
}


/*! \return the index of the current task
 */
int VerifyEngine::currentTask() const
{
    return m_currentTask;
}


/*! \return the data stored under the given role for the task referred to by the index
 */
QVariant VerifyEngine::taskData(int /*task*/, int role) const
{
    if (role == Qt::DisplayRole) {
        return QVariant(tr("Verifying the current backup"));
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
WorkerStatus VerifyEngine::status() const
{
    WorkerStatus st;
    st.timestamp  = QDateTime::currentDateTime();
    st.completion = ((qreal)m_validateTraverser.processed()) / qMax(m_metaInfo.dataSize(), (qint64)1);
    st.transferred = m_validateTraverser.transferred();

    return st;
}

/*! Slot called each time, the "Verify Backup" button is pressed
 *
 * Attention: this will run in a different thread scope
 */
void VerifyEngine::start()
{
    reset();
    m_currentTask = 0;

    QString empty;
    Backup &backup = Backup::instance();

    QString currentBackup = SearchLatestBackupDir(backup.location());
    m_validateTraverser.reset();
    emit started();

    m_metaInfo.load(currentBackup + "/metainfo") ;
    m_validateTraverser.addIncludes(backup.config().includes());
    m_validateTraverser.addExcludes(backup.config().excludes());
    m_validateTraverser.addExcludes(currentBackup + "/metainfo");
    m_validateTraverser.addExcludes(currentBackup + "/digests");
    m_validateTraverser.setBackupPath(empty);
    QByteArray checksum = m_validateTraverser.digests().load(currentBackup + "/digests");
    if (checksum == m_metaInfo.checksum()) {
        try {

            m_validateTraverser.traverse();
            m_validateTraverser.evaluate(m_metaInfo);
            m_currentTask = -1; // disable highlighted task

            m_metaInfo.save(currentBackup + "/metainfo");
            emit finished();

        } catch (ApplicationException &e) {
            buildFailureHint(e);
            emit failed();
        }
    } else {
        m_failureHint = tr("The content of the digests file are not trustable because "
                           "it's checksum does not match the expected checksum!<br>"
                           "A backup verification is not possible!<br>"
                           "The latest backup snapshot status will be set to 'Unknown'.");
        m_metaInfo.setQuality(SnapshotMetaInfo::Unknown);
        m_metaInfo.save(currentBackup + "/metainfo");
        emit failed();
    }

    m_validateTraverser.digests().clear();
}


/*! Indicates, that the worker should terminate it's execution.
 *
 * Attention: abort() can not be called via event loop, because
 * the thread blocks its event queue. Thus, do not "connect"!
 */
void VerifyEngine::abort()
{
    qDebug() << "VerifyEngine: abort requested";
    m_abort = true;
    m_validateTraverser.abort();
}
