/*
 * SnapshotListModel.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2013  Marc Weidler <marc.weidler@web.de>,
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


#include "SnapshotListModel.h"
#include "SnapshotMetaInfo.h"
#include "Utilities.h"

#include <QDir>
#include <QIcon>
#include <QLocale>


/*! Constructs a new backup history list object
 *
 * \param parent the parent UI element
 */
SnapshotListModel::SnapshotListModel(QObject *parent) : QAbstractTableModel(parent)
{
    clear();

    m_headerLabels << tr("Name") <<  tr("Files") << tr("Size") <<
                   tr("Last modified") << tr("Status") << tr("Checksum");
}


/*! Destructor
 */
SnapshotListModel::~SnapshotListModel()
{
    clear();
}


void SnapshotListModel::clear()
{
    beginResetModel();
    m_historyList.clear();
    endResetModel();
}


int SnapshotListModel::count() const
{
    return m_historyList.count();
}


bool SnapshotListModel::isEmpty() const
{
    return m_historyList.isEmpty();
}


const Snapshot &SnapshotListModel::operator[](int i) const
{
    return m_historyList[i];
}


const Snapshot &SnapshotListModel::at(int i) const
{
    return m_historyList[i];
}


const Snapshot &SnapshotListModel::first() const
{
    return m_historyList.first();
}


/*! Returns the number of rows of the model, means backup history entries
 *
 * \return number of rows
 */
int SnapshotListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_historyList.size();
}


/*! Return number of columns in the table of the model
 *
 * \return number of columns
 */
int SnapshotListModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_headerLabels.size();
}


/*! \return the title of the headers
 */
QVariant SnapshotListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_headerLabels[section];
    }

    if (role == Qt::TextAlignmentRole) {
        if (section == 1 || section == 2) {
            return QVariant(Qt::AlignRight);
        } else {
            return QVariant(Qt::AlignLeft);
        }
    }

    return QVariant();
}


/*! \return the data to displayed in the tabe
 */
QVariant SnapshotListModel::data(const QModelIndex &index, int role) const
{
    const Snapshot &snapshot = m_historyList[index.row()];

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 1 || index.column() == 2) {
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        } else {
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    if (role == Qt::DecorationRole && index.column() == 4) {
        switch (snapshot.metaInfo().quality()) {
            default:
            case SnapshotMetaInfo::Unknown:
                return QIcon::fromTheme("dialog-question");
                break;
            case SnapshotMetaInfo::Partial:
                return QIcon::fromTheme("dialog-warning");
                break;
            case SnapshotMetaInfo::Complete:
                return QIcon::fromTheme("ok", QIcon(":/images/ok.png"));
                break;
            case SnapshotMetaInfo::Reliable:
                return QIcon::fromTheme("starred", QIcon(":/images/starred.png"));
                break;
        }
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QVariant(snapshot.name());
                break;

            case 1:
                return QVariant(snapshot.metaInfo().fileCount());
                break;

            case 2:
                return formatSize(snapshot.metaInfo().dataSize());
                break;

            case 3:
                return QVariant(QLocale().toString(snapshot.modificationTime(), tr("dddd, dd-MMM-yyyy hh:mm:ss")));
                // DE: "dd.MM.yyyy hh:mm:ss"
                break;
            case 4:
                switch (snapshot.metaInfo().quality()) {
                    default:
                    case SnapshotMetaInfo::Unknown:
                        return tr("Unknown");
                        break;
                    case SnapshotMetaInfo::Partial:
                        return tr("Partial");
                        break;
                    case SnapshotMetaInfo::Complete:
                        return tr("Complete");
                        break;
                    case SnapshotMetaInfo::Reliable:
                        return tr("Reliable");
                        break;
                }
                break;

            case 5:
                return QVariant(snapshot.metaInfo().checksum());
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
void SnapshotListModel::investigate(const QString &origin)
{
    QDir dir(origin);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);

    beginResetModel();
    m_historyList.clear();

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++) {
        const QFileInfo &fileInfo = list.at(i);

        // read only snapshot directory names (starting with @-sign)
        if (!fileInfo.fileName().startsWith("@")) {
            continue;
        }

        Snapshot snapshot;
        snapshot.setModificationTime(fileInfo.lastModified());
        snapshot.setLocation(fileInfo.absoluteFilePath());
        snapshot.setName(fileInfo.fileName());

        SnapshotMetaInfo metaInfo;
        if (metaInfo.load(fileInfo.absoluteFilePath() + "/metainfo")) {
            snapshot.setMetaInfo(metaInfo);
        }

        m_historyList.append(snapshot);
    }

    endResetModel();
}

