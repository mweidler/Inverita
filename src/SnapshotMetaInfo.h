/*
 * SnapshotMetaInfo.hpp
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

#ifndef HEADER_SNAPSHOTMETAINFO_INC
#define HEADER_SNAPSHOTMETAINFO_INC

#include <QtCore>
#include <QString>
#include <QStringList>

#include "ApplicationException.h"

class SnapshotMetaInfo : public QObject
{
public:
    SnapshotMetaInfo();
    SnapshotMetaInfo(const SnapshotMetaInfo &other);
    SnapshotMetaInfo &operator= (const SnapshotMetaInfo & other);

    void reset();
    bool Load(QString filename);
    void Save(QString filename);

    qint64 numberOfFiles() const;
    void setNumberOfFiles(qint64 count);

    qint64 sizeOfFiles() const;
    void setSizeOfFiles(qint64 count);

protected:
    int  m_files;
    int  m_totalSize;
};

#endif
