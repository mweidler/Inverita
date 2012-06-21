/*
 * DriveCapacityWatcher.h
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

#ifndef HEADER_DRIVECAPACITYWATCHER_INC
#define HEADER_DRIVECAPACITYWATCHER_INC

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
class DriveCapacityWatcher : public WorkerEngine
{
    Q_OBJECT

public:
    DriveCapacityWatcher(AbstractDriveCapacityModel *capacityModel, BackupHistoryList *historyListModel);

    WorkerStatus status();
    void select(const QString &backupPath);
    void setAutoDeleteEnabled(bool autoDelete);

signals:
    void refresh();

public slots:
    void watch();
    void start();
    void abort();

protected:
    bool                       m_autoDelete;
    QString                    m_backupRootPath;
    EraseTraverser             m_eraseTraverser;
    QTimer                     *m_timer;
    AbstractDriveCapacityModel *m_capacityModel;    //!< the model containing drive space information
    BackupHistoryList          *m_historyListModel;
};

#endif
