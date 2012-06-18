/*
 * BackupHistoryList.cpp
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


#include "BackupHistoryList.h"
#include "SnapshotMetaInfo.h"
#include "Utilities.h"

/*! Constructs a new backup history list object
 *
 * /param parent the parent UI element
 */
BackupHistoryList::BackupHistoryList(QObject *parent) : QAbstractTableModel(parent)
{
    m_headerLabels << tr("Name") <<
                   tr("Execution date/time") <<
                   tr("Files") <<
                   tr("Size") <<
                   tr("Execution status");
    clear();
}


/*! Destructor
 */
BackupHistoryList::~BackupHistoryList()
{
    clear();
}


/*! Returns the number of rows of the model, means backup history entries
 *
 * \return number of rows 
 */
int BackupHistoryList::rowCount(const QModelIndex & /*parent*/) const
{
    return size();
}


/*! Return number of columns in the table of the model
 *
 * \return number of columns
 */
int BackupHistoryList::columnCount(const QModelIndex & /*parent*/) const
{
    return m_headerLabels.size();
}


/*! \return the title of the headers
 */
QVariant BackupHistoryList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_headerLabels[section];
    }

    if (role == Qt::TextAlignmentRole) {
        if (section == 2 || section == 3) {
            return QVariant(Qt::AlignRight);
        } else {
            return QVariant(Qt::AlignLeft);
        }
    }

    return QVariant();
}


/*! \return the data to displayed in the tabe
 */
QVariant BackupHistoryList::data(const QModelIndex &index, int role) const
{
    const BackupHistoryEntry &entry = this->at(index.row());

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 2 || index.column() == 3) {
            return QVariant(Qt::AlignRight);
        } else {
            return QVariant(Qt::AlignLeft);
        }
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QVariant(entry.name);
                break;

            case 1:
                return QVariant(entry.execution.toString(Qt::SystemLocaleLongDate)); //entry.execution);
                break;

            case 2:
                return QVariant(entry.files);
                break;

            case 3:
                return ScaleToSiPrefix(entry.totalSize);
                break;

            case 4:
                return QString("Unknown");
                break;

            default:
                return QString("Row%1, Column%2")
                       .arg(index.row() + 1)
                       .arg(index.column() + 1);
        }
    }

    return QVariant();
}


/*! Fills the backup history list by analyzing all directories in the given
 *  directory (backup root directory. If the directory has valid metadata,
 *  then the directoty will see accepted as a valid backup snapshot and
 *  added to the list.
 *
 * \param origin backup origin root directory
 */
void BackupHistoryList::investigate(const QString &origin)
{
    QDir dir(origin);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Time | QDir::Reversed);

    beginResetModel();
    this->clear();

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);

        qDebug() << "Investigate" << fileInfo.absoluteFilePath();
        SnapshotMetaInfo metaInfo;
        if (metaInfo.Load(fileInfo.absoluteFilePath() + "/" + "metainfo")) {
            BackupHistoryEntry entry;
            entry.execution = fileInfo.lastModified();
            entry.files = metaInfo.numberOfFiles();
            entry.totalSize = metaInfo.sizeOfFiles();
            entry.location = fileInfo.absoluteFilePath();
            entry.name = fileInfo.fileName();
            this->append(entry);
        }
    }

    endResetModel();
}
