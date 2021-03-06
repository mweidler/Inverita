/*
 * SnapshotMetaInfo.hpp
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

#ifndef HEADER_SNAPSHOTMETAINFO_INC
#define HEADER_SNAPSHOTMETAINFO_INC

#include "ApplicationException.h"

#include <QString>
#include <QByteArray>
#include <QObject>


/*! Container class that contains meta information of the snapshot, e.g.
 *  size, files, quality, etc.
 */
class SnapshotMetaInfo : public QObject
{
    Q_OBJECT

public:
    enum Quality { Unknown, Partial, Complete, Reliable };

    SnapshotMetaInfo();
    SnapshotMetaInfo(const SnapshotMetaInfo &other);
    SnapshotMetaInfo &operator= (const SnapshotMetaInfo &other);

    void reset();
    bool load(const QString &filename);
    void save(const QString &filename);

    QByteArray checksum() const;
    void setChecksum(const QByteArray &checksum);

    qint64 fileCount() const;
    void setFileCount(qint64 count);

    qint64 dataSize() const;
    void setDataSize(qint64 count);

    Quality quality() const;
    void setQuality(Quality status);

protected:
    qint64     m_filecount;
    qint64     m_dataSize;
    QByteArray m_checksum;
    Quality    m_quality;
};

#endif
