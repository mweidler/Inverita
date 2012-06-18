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

#include <QString>
#include "BackupListModel.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>


/*! Constructs a new backup list model.
 *
 * \param  parent the parent UI control element
 */
BackupListModel::BackupListModel(QObject *parent) : QAbstractListModel(parent)
{  // TODO is parent really needed?
    m_list.clear();

    connect(this, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(onDataChanged()));
}


/*! Destructor
 */
BackupListModel::~BackupListModel()
{
    m_list.clear();
}

/*! \return the backup list
 */
BackupList BackupListModel::backupList()
{
    return m_list;
}


/*! \return the flags
 */
Qt::ItemFlags BackupListModel::flags(const QModelIndex &index) const
{// TODO: is this method really needed?
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool BackupListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    fprintf(stderr, "BackupList setData %d\n", index.row());

    if (index.isValid() && role == Qt::EditRole) {

        BackupEntry entry;
        entry.origin = value.toString();

        m_list.replace(index.row(), entry);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

QVariant BackupListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_list.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return m_list.at(index.row()).origin;
    } else {
        return QVariant();
    }
}

int BackupListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_list.size();
}

bool BackupListModel::insertRows(int position, int rows, const QModelIndex & /*parent*/)
{
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        qDebug() << "BackupList insert: " << position;

        BackupEntry entry;
        m_list.insert(position, entry);
    }

    endInsertRows();
    return true;
}

bool BackupListModel::removeRows(int position, int rows, const QModelIndex & /*parent*/)
{
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        m_list.removeAt(position);
    }

    endRemoveRows();
    return true;
}

void BackupListModel::onDataChanged()
{
    qDebug() <<  "onDataChanged called";
    Save();
}


/*! Read config into selector choice.
 *
 * \param organization the organisation/filename for the configuration storage
 */
int BackupListModel::Load(const QString &organization)
{
    m_list.clear();

    QSettings settings(organization);

    int size = settings.beginReadArray("Backups");
    for (int i = 0; i < size; ++i) {
        fprintf(stderr, "Reading BackupList %d\n", i);
        settings.setArrayIndex(i);
        BackupEntry entry;
        entry.origin = settings.value("origin").toString();
        m_list.append(entry);
    }
    settings.endArray();

    m_organization = organization;

    return m_list.size();
}


/*! Write selector choice content to config file.
 */
void BackupListModel::Save()
{
    QSettings settings(m_organization);

    settings.beginWriteArray("Backups");
    for (int i = 0; i < m_list.size(); ++i) {
        qDebug() << "Write BackupList " << i << " in  " << m_organization;
        settings.setArrayIndex(i);
        settings.setValue("origin", m_list[i].origin);
    }
    settings.endArray();
}


/*! Write selector choice content to config file.
 *
 * \param organization the organisation/filename for the configuration storage
 */
void BackupListModel::SaveAs(const QString &organization)
{
    QSettings settings(organization);

    settings.beginWriteArray("Backups");
    for (int i = 0; i < m_list.size(); ++i) {
        qDebug() << "Write BackupList" <<  i;
        settings.setArrayIndex(i);
        settings.setValue("origin", m_list[i].origin);
    }
    settings.endArray();
}
