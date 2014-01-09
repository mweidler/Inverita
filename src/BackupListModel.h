/*
 * BackupListModel.h
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

#ifndef HEADER_BACKUPLISTMODEL_INC
#define HEADER_BACKUPLISTMODEL_INC

#include "Backup.h"
#include <QAbstractListModel>


/*! Container for meta information and digest value.
 */
typedef struct BackupEntry {
    QString            label;
    QString            origin;
    Backup::Encryption encryption;
    QString            password;
} BackupEntry;


/*! Base type storage definition
 */
typedef QList<BackupEntry> BaseBackupList;
typedef QListIterator<BackupEntry> BackupListIterator;


/*! Container class for storing and handling validation information.
 */
class BackupListModel : public BaseBackupList, public QAbstractListModel
{
public:
    BackupListModel(QObject *parent = 0);
    virtual ~BackupListModel();

    // model<->view handling
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int setEntry(const BackupEntry &entry);
    int load(const QString &organization);
    void save();
    void saveAs(const QString &organization);

private:
    QString m_organization;
};

#endif // ! HEADER_BACKUPLISTMODEL_INC
