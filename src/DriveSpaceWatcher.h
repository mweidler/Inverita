/*
 * DriveSpaceWatcher.h
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

#ifndef HEADER_DRIVESPACEWATCHER_INC
#define HEADER_DRIVESPACEWATCHER_INC

#include <QString>
#include <QStringList>
#include <QTimer>

#include "WorkerEngine.h"
#include "EraseTraverser.h"
#include "SnapshotMetaInfo.h"
#include "AbstractDriveCapacityModel.h"
#include "BackupHistoryList.h"

/*! Handles the whole job of erasing a backup snapshot.
 */
class DriveSpaceWatcher : public WorkerEngine
{
    Q_OBJECT

public:
    DriveSpaceWatcher(AbstractDriveCapacityModel *capacityModel, BackupHistoryList *historyListModel);

    WorkerStatus status();
    void select(const QString &backupPath);

public slots:
    void update();
    void start();
    void abort();

protected:
    QTimer                    *m_timer;
    QString                    m_backupRootPath;
    QString                    m_snapshotName;
    EraseTraverser             m_eraseTraverser;
    SnapshotMetaInfo           m_metaInfo;
    AbstractDriveCapacityModel *m_capacityModel;    //!< the model containing drive space information
    BackupHistoryList          *m_historyListModel;
};

#endif
