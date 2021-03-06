/*
 * SnapshotListModel.h
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2014  Marc Weidler <marc.weidler@web.de>,
 *                          Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                          All rights reserved.
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

#ifndef HEADER_SNAPSHOTLISTMODEL_INC
#define HEADER_SNAPSHOTLISTMODEL_INC

#include "Snapshot.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QListIterator>
#include <QAbstractTableModel>


/*! Base type storage definition
 */
typedef QList<Snapshot> BaseBackupHistoryList;
typedef QListIterator<Snapshot> BackupHistoryListIterator;


/*! Model class for storing and handling backup history.
 */
class SnapshotListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SnapshotListModel(QObject *parent = 0);
    virtual ~SnapshotListModel();

    void clear();
    int count() const;
    const Snapshot &operator[](int i) const;
    const Snapshot &at(int i) const;
    const Snapshot &first() const;
    bool  isEmpty() const;

    // model<->view handling
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void investigate(const QString &origin);

protected:
    QStringList           m_headerLabels;
    BaseBackupHistoryList m_historyList;
};

#endif // ! HEADER_SNAPSHOTLISTMODEL_INC
