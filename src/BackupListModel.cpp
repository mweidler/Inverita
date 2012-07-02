/*
 * BackupList.cpp
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


#include "BackupListModel.h"


/*! Constructs a new backup list model.
 *
 * \param  parent the parent UI control element
 */
BackupListModel::BackupListModel(QObject *parent) : QAbstractListModel(parent)
{
   clear();
}


/*! Destructor
 */
BackupListModel::~BackupListModel()
{
    clear();
}


QVariant BackupListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= this->size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return this->at(index.row()).origin;
    } else {
        return QVariant();
    }
}

int BackupListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return this->size();
}


/*! Appends a new BackupEntry in the model list.
 *
 * \param the new BackupEntry to be inserted
 */
void BackupListModel::appendEntry(const BackupEntry &entry)
{
    beginResetModel();
    this->append(entry);
    endResetModel();
    Save();
}


/*! Read config into selector choice.
 *
 * \param organization the organisation/filename for the configuration storage
 */
int BackupListModel::Load(const QString &organization)
{
    this->clear();

    QSettings settings(organization);

    beginResetModel();
    int size = settings.beginReadArray("Backups");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        BackupEntry entry;
        entry.origin = settings.value("origin").toString();
        this->append(entry);
    }
    settings.endArray();
    endResetModel();

    m_organization = organization;

    return this->size();
}


/*! Write selector choice content to config file.
 */
void BackupListModel::Save()
{
    SaveAs(m_organization);
}


/*! Write selector choice content to config file.
 *
 * \param organization the organisation/filename for the configuration storage
 */
void BackupListModel::SaveAs(const QString &organization)
{
    QSettings settings(organization);

    settings.beginWriteArray("Backups");
    for (int i = 0; i < this->size(); ++i) {
        qDebug() << "Write BackupList" <<  i;
        settings.setArrayIndex(i);
        settings.setValue("origin", this->at(i).origin);
    }
    settings.endArray();
}
