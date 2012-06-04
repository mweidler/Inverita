/*
 * BackupList.cpp
 *
 * Copyright (c) 2012, Marc Weidler <marc.weidler@web.de>
 * Ulrichstr. 12/1, 71672 Marbach, Germany
 * All rights reserved.
 *
 * 'ALVARA Personal Backup' is licensed under the BSD license (simplified, 2-clause).
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QString>
#include "BackupListModel.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>


/*****************************************************************************
 * Constructor
 *****************************************************************************/
BackupListModel::BackupListModel(QObject *parent) : QAbstractListModel(parent)
{
    m_list.clear();

    connect(this, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(onDataChanged()));
}

/*****************************************************************************
 * Destructor
 *****************************************************************************/
BackupListModel::~BackupListModel()
{
    m_list.clear();
}

BackupList BackupListModel::backupList()
{
    return m_list;
}

Qt::ItemFlags BackupListModel::flags(const QModelIndex &index) const
{
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
        fprintf(stderr, "BackupList insert %d\n", position);

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
    fprintf(stderr, "onDataChanged called\n");
    Save();
}

/*******************************************************************************
* Read config into selector choice.
*******************************************************************************/
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


/*******************************************************************************
* Write selector choice content to config file.
*******************************************************************************/
void BackupListModel::Save()
{
    QSettings settings(m_organization);

    settings.beginWriteArray("Backups");
    for (int i = 0; i < m_list.size(); ++i) {
        std::cerr << "Write BackupList " << i << " in  " << m_organization.toStdString() << "\n";
        settings.setArrayIndex(i);
        settings.setValue("origin", m_list[i].origin);
    }
    settings.endArray();
}


/*******************************************************************************
* Write selector choice content to config file.
*******************************************************************************/
void BackupListModel::SaveAs(const QString &organization)
{
    QSettings settings(organization);

    settings.beginWriteArray("Backups");
    for (int i = 0; i < m_list.size(); ++i) {
        fprintf(stderr, "Write BackupList %d\n", i);
        settings.setArrayIndex(i);
        settings.setValue("origin", m_list[i].origin);
    }
    settings.endArray();
}
