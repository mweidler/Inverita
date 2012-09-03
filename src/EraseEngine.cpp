/*
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
#include <QDebug>


/*! Constructs a new erase engine object.
 */
EraseEngine::EraseEngine()
{
    reset();

    m_descriptions << tr("Deleting the selected backup snapshot");
}


/*! Returns the current \em WorkerStatus of this engine,
 *  containing \em completion and \em processed bytes.
 *
 *  The worker status is requested by the progress dialog
 *  to present the current job status to the user.
 *
 *  \return the current \em WorkerStatus
 */
WorkerStatus EraseEngine::status()
{
    qint64 expectedFiles = qMax(m_metaInfo.numberOfFiles(), (qint64)1);

    WorkerStatus st;
    st.timestamp  = QDateTime::currentDateTime();
    st.completion = expectedFiles ? ((qreal)m_eraseTraverser.files() / expectedFiles) : 0;
    st.transferred = 0;

    return st;
}


/*! Is called each time a new backup has selected (by the user)
 *  to inform the engine of a new backup root path.
 *
 * \param backupPath the new backup root path
 */
void EraseEngine::select(const QString &snapshotName)
{
    m_snapshotName = snapshotName;
}


/*! This slot is called each time, the "Erase Backup snapshot" button is pressed
 *  by the user and a backup snapshot shall be erased.
 *
 *  \em Attention: this method will run in it's own thread scope.
 */
void EraseEngine::start()
{
    reset();
    m_eraseTraverser.reset();
    emit started();

    m_metaInfo.load(m_snapshotName + "/metainfo");

    // remove metainfo and digests of the snapshot "manually" to
    // invalidate whole snapshot. They were not counted on meta data creation.
    QFile::remove(m_snapshotName + "/metainfo");
    QFile::remove(m_snapshotName + "/digests");

    m_eraseTraverser.addIncludes(m_snapshotName);

    try {
        m_eraseTraverser.traverse();
        m_currentTask = -1; // disable highlighted task

    } catch (ApplicationException &e) {
        buildFailureHint(e);
        emit failed();
        return;
    }

    emit finished();
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
 *
 *  \em Attention: Erase can not be aborted.
 */
void EraseEngine::abort()
{
    qDebug() << "EraseEngine: abort requested but erase is not allowed to abort";
}
