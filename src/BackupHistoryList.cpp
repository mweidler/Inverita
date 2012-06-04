/*
 * BackupHistoryList.cpp
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
#include "BackupHistoryList.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>


/*******************************************************************************
 * Constructor
 ******************************************************************************/
BackupHistoryList::BackupHistoryList(QObject *parent) : QAbstractTableModel(parent)
{
    m_headerLabels << tr("Name") <<
                   tr("Execution date") <<
                   tr("Number of files") <<
                   tr("Size") <<
                   tr("Execution status");
    clear();
}


/*******************************************************************************
 * Destructor
 ******************************************************************************/
BackupHistoryList::~BackupHistoryList()
{
    clear();
}


/*******************************************************************************
 * Return number of rows, means backup history entries
 ******************************************************************************/
int BackupHistoryList::rowCount(const QModelIndex & /*parent*/) const
{
    return size();
}


/*******************************************************************************
 * Return number of columns in the table
 ******************************************************************************/
int BackupHistoryList::columnCount(const QModelIndex & /*parent*/) const
{
    return m_headerLabels.size();
}


/*******************************************************************************
 * Return the title of the headers
 ******************************************************************************/
QVariant BackupHistoryList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_headerLabels[section];
    }

    return QVariant();
}


/*******************************************************************************
 * Return the data to displayed in the tabe
 ******************************************************************************/
QVariant BackupHistoryList::data(const QModelIndex &index, int role) const
{
    const BackupHistoryEntry &entry = this->at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QVariant(entry.name);
                break;

            case 1:
                return QVariant(entry.execution);
                break;

            case 2:
                return QVariant(entry.files);
                break;

            default:
                return QString("Row%1, Column%2")
                       .arg(index.row() + 1)
                       .arg(index.column() + 1);
        }
    }
    return QVariant();
}


/*******************************************************************************
 * Investigates the backup history
 ******************************************************************************/
void BackupHistoryList::investigate(QString &origin)
{
    QDir dir(origin);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Time | QDir::Reversed);

    beginResetModel();
    this->clear();

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);

        BackupHistoryEntry entry;
        entry.execution = fileInfo.lastModified();
        entry.files = i;
        entry.totalSize = i * i;
        entry.location = fileInfo.absoluteFilePath();
        entry.name = fileInfo.fileName();
        this->append(entry);
    }

    endResetModel();

    //QModelIndex topLeft = createIndex(0,0);
    //QModelIndex bottomRight = createIndex(m_headerLabels.size()-1,size()-1);
    //emit a signal to make the view reread identified data
    //emit dataChanged(topLeft, bottomRight);
}
