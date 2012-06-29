/*
 * Snapshot.h
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

#ifndef HEADER_SNAPSHOT_INC
#define HEADER_SNAPSHOT_INC


#include "SnapshotMetaInfo.h"

#include <QDateTime>

/*! Container class for storing snapshot data
 */
class Snapshot : public QObject
{
public:
    enum SnapshotStatus { Invalid, Valid };

    Snapshot();
    Snapshot(const Snapshot &other);
    Snapshot &operator= (const Snapshot &other);

    void reset();

    QDateTime modificationTime() const;
    void  setModificationTime(QDateTime modificationTime);

    QString origin() const;
    void  setOrigin(QString origin);

    QString location() const;
    void  setLocation(QString location);

    QString name() const;
    void  setName(QString name);

    SnapshotStatus status() const;
    void  setStatus(SnapshotStatus status);

    SnapshotMetaInfo metaInfo() const;
    void setMetaInfo(SnapshotMetaInfo &metainfo);

protected:
    QDateTime        m_execution;
    QString          m_origin;
    QString          m_location;
    QString          m_name;
    SnapshotStatus   m_status;
    SnapshotMetaInfo m_metaInfo;
};

#endif
