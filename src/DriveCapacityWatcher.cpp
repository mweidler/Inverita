/*
 * DriveCapacityWatcher.cpp
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


#include "DriveCapacityWatcher.h"
#include "Utilities.h"


/*! Constructs a new drivespace watcher object.
 */
DriveCapacityWatcher::DriveCapacityWatcher(AbstractDriveCapacityModel *capacityModel, SnapshotListModel *historyListModel)
{
    m_capacityModel = capacityModel;
    m_snapshotListModel = historyListModel;

    reset();
    setAutoDeleteEnabled(false);
    m_timer = new QTimer(this);
    m_timer->setInterval(10000);
    m_timer->start();

    connect(this, SIGNAL(refresh()), m_capacityModel, SLOT(refresh()));
    connect(m_timer, SIGNAL(timeout()), m_capacityModel, SLOT(refresh()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(watch()));
}


/*! Returns the current \em WorkerStatus of this engine,
 *  containing \em completion and \em processed bytes.
 *
 *  The worker status is requested by the progress dialog
 *  to present the current job status to the user.
 *
 *  \return the current \em WorkerStatus
 */
WorkerStatus DriveCapacityWatcher::status()
{
    WorkerStatus st;
    st.timestamp = QDateTime::currentDateTime();
    st.completion = 0;
    st.processed  = 0;

    return st;
}


/*! Is called each time a new backup has selected (by the user)
 *  to inform the engine of a new backup root path.
 *
 * \param backupPath the new backup root path
 */
void DriveCapacityWatcher::select(const QString &backupPath)
{
    m_backupRootPath = backupPath;
}


void DriveCapacityWatcher::setAutoDeleteEnabled(bool autoDelete)
{
    m_autoDelete = autoDelete;
}


/*! This slot is called periodically and watches the free space on the drive.
 *
 *  \em Attention: this method will run in it's own thread scope.
 */
void DriveCapacityWatcher::watch()
{
    if (m_capacityModel->capacity() >= 0.05 || m_autoDelete == false) {
        return;
    }

    reset();
    SnapshotListModel historyList(m_snapshotListModel);

    try {
        for (int i = 0; i < historyList.size() && m_capacityModel->capacity() < 0.1 && !m_abort; i++) {
            if (historyList[i].status() != Snapshot::Valid) {
                break;
            }

            QString snapshotName = m_backupRootPath + "/" + historyList[i].name();

            m_eraseTraverser.reset();
            m_eraseTraverser.addIncludes(snapshotName);

            // force metainfo deletion as first file of the snapshot to
            // invalidate whole snapshot.
            m_eraseTraverser.onFile(snapshotName + "/" + "metainfo");
            m_eraseTraverser.traverse();
            emit refresh();
        }

    } catch (ApplicationException &e) {
        m_autoDelete = false;
        buildFailureHint(e);
        emit failed();
        return;
    }
}

void DriveCapacityWatcher::start()
{
    qDebug() << "DriveCapacityWatcher::start()" << "not used";
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
void DriveCapacityWatcher::abort()
{
    qDebug() << "DriveCapacityWatcher: abort requested but erase is not allowed to abort";
}
