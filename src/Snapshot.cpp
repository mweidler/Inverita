/*
 * Snapshot.cpp
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


#include <QtCore>
#include <QString>
#include <QStringList>

#include "Snapshot.h"


Snapshot::Snapshot()
{
    reset();
}

void Snapshot::reset()
{
    m_execution = QDateTime::currentDateTime();
    m_origin.clear();
    m_location.clear();
    m_name.clear();
    m_metaInfo.reset();
}

QDateTime Snapshot::modificationTime()
{
    return m_execution;
}

void  Snapshot::setModificationTime(const QDateTime &modificationTime)
{
    m_execution = modificationTime;
}

QString Snapshot::origin()
{
    return m_origin;
}

void  Snapshot::setOrigin(const QString &origin)
{
    m_origin = origin;
}

QString Snapshot::location()
{
    return m_location;
}

void  Snapshot::setLocation(const QString &location)
{
    m_location = location;
}

QString Snapshot::name()
{
    return m_name;
}

void  Snapshot::setName(const QString &name)
{
    m_name = name;
}

SnapshotMetaInfo & Snapshot::metaInfo()
{
    return m_metaInfo;
}
