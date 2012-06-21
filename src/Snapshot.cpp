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

Snapshot::Snapshot(const Snapshot &other) : QObject()
{
    m_execution = other.m_execution;
    m_origin = other.m_origin;
    m_location = other.m_location;
    m_name = other.m_name;
    m_status = other.m_status;
    m_metaInfo = other.m_metaInfo;
}

Snapshot &Snapshot::operator= (const Snapshot &other)
{
    m_execution = other.m_execution;
    m_origin = other.m_origin;
    m_location = other.m_location;
    m_name = other.m_name;
    m_status = other.m_status;
    m_metaInfo = other.m_metaInfo;

    return *this;
}

void Snapshot::reset()
{
    m_execution = QDateTime::currentDateTime();
    m_origin.clear();
    m_location.clear();
    m_name.clear();
    m_status = Snapshot::Invalid;
    m_metaInfo.reset();
}

QDateTime Snapshot::modificationTime() const
{
    return m_execution;
}

void  Snapshot::setModificationTime(QDateTime modificationTime)
{
    m_execution = modificationTime;
}

QString Snapshot::origin() const
{
    return m_origin;
}

void  Snapshot::setOrigin(QString origin)
{
    m_origin = origin;
}

QString Snapshot::location() const
{
    return m_location;
}

void Snapshot::setLocation(QString location)
{
    m_location = location;
}

QString Snapshot::name() const
{
    return m_name;
}

void  Snapshot::setName(QString name)
{
    m_name = name;
}

Snapshot::SnapshotStatus Snapshot::status() const
{
    return m_status;
}

void  Snapshot::setStatus(SnapshotStatus status)
{
    m_status = status;
}

SnapshotMetaInfo Snapshot::metaInfo() const
{
    return m_metaInfo;
}

void Snapshot::setMetaInfo(SnapshotMetaInfo &metainfo)
{
    m_metaInfo = metainfo;
}
