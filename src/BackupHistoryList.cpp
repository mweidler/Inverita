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
 * \param parent the parent UI element
 */
BackupHistoryList::BackupHistoryList(QObject *parent) : QAbstractTableModel(parent)
{
    m_headerLabels << tr("Name") <<
                   tr("Last modified") <<
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
    const Snapshot &snapshot = this->at(index.row());

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
            return QVariant(snapshot.name());
                break;

            case 1:
                return QVariant(snapshot.modificationTime().toString(Qt::SystemLocaleLongDate)); //entry.execution);
                break;

            case 2:
                return QVariant(snapshot.metaInfo().numberOfFiles());
                break;

            case 3:
                return ScaleToSiPrefix(snapshot.metaInfo().sizeOfFiles());
                break;

            case 4:
                if (snapshot.status() == Snapshot::Valid) return tr("Valid");
                if (snapshot.status() == Snapshot::Invalid) return tr("Invalid");
                return QString("Unknown");
                break;

            default:
                break;
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

        Snapshot snapshot;
        snapshot.setModificationTime(fileInfo.lastModified());
        snapshot.setLocation(fileInfo.absoluteFilePath());
        snapshot.setName(fileInfo.fileName());

        SnapshotMetaInfo metaInfo;
        if (metaInfo.Load(fileInfo.absoluteFilePath() + "/" + "metainfo")) {
            snapshot.setMetaInfo(metaInfo);
            snapshot.setStatus(Snapshot::Valid);
        } else {
            snapshot.setStatus(Snapshot::Invalid);
        }

        this->append(snapshot);
    }

    endResetModel();
}
