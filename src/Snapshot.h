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

class Snapshot : public QObject
{
public:
    Snapshot();

    void reset();

    QDateTime modificationTime();
    void  setModificationTime(const QDateTime &modificationTime);

    QString origin();
    void  setOrigin(const QString &origin);

    QString location();
    void  setLocation(const QString &location);

    QString name();
    void  setName(const QString &name);

    SnapshotMetaInfo &metaInfo();

protected:
    QDateTime        m_execution;
    QString          m_origin;
    QString          m_location;
    QString          m_name;
    SnapshotMetaInfo m_metaInfo;
};

#endif
