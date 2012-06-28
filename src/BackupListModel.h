/*
 * BackupListModel.h
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

#ifndef HEADER_BACKUPLISTMODEL_INC
#define HEADER_BACKUPLISTMODEL_INC

#include <QtCore>
#include <QString>
#include <QList>
#include <QListIterator>
#include <QStringListModel>


/*! Container for meta information and hash value.
 */
typedef struct BackupEntry {
    QString     origin;
    int         files;
} BackupEntry;


/*! Base type storage definition
 */
typedef QList<BackupEntry> BackupList;
typedef QListIterator<BackupEntry> BackupListIterator;


/*! Container class for storing and handling validation information.
 */
class BackupListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    BackupListModel(QObject *parent = 0);
    virtual ~BackupListModel();

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    void setBackupList(const BackupList &list);
    BackupList backupList();

    int Load(const QString &organization);
    void Save();
    void SaveAs(const QString &organization);

public slots:
    void onDataChanged();

protected:

private:
    BackupList m_list;
    QString    m_organization;
};

#endif // ! HEADER_BACKUPLISTMODEL_INC
