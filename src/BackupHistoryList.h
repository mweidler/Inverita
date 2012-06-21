/*
 * BackupHistoryList.h
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

#ifndef HEADER_BACKUPHISTORYLIST_INC
#define HEADER_BACKUPHISTORYLIST_INC

#include "Snapshot.h"

#include <QtCore>
#include <QString>
#include <QList>
#include <QListIterator>
#include <QFileInfo>
#include <QAbstractTableModel>
#include <sys/stat.h>


/*! Base type storage definition
 */
typedef QList<Snapshot> BaseBackupHistoryList;
typedef QListIterator<Snapshot> BackupHistoryListIterator;


/*! Model class for storing and handling backup history.
 */
class BackupHistoryList : public BaseBackupHistoryList, public QAbstractTableModel
{
public:
    BackupHistoryList(QObject *parent = 0);
    virtual ~BackupHistoryList();

    // model<->view handling
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void investigate(const QString &origin);
};

#endif // ! HEADER_BACKUPHISTORYLIST_INC
