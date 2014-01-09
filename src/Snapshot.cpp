/*
 * Snapshot.cpp
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


#include "Snapshot.h"

#include <QString>
#include <QStringList>


/*! Constructs a new snapshot object.
 */
Snapshot::Snapshot()
{
    reset();
}


/*! Copy constructor: constructs a new snapshot object from another one.
 *
 * \param other the other snapshot object
 */
Snapshot::Snapshot(const Snapshot &other) : QObject()
{
    m_execution = other.m_execution;
    m_origin = other.m_origin;
    m_location = other.m_location;
    m_name = other.m_name;
    m_metaInfo = other.m_metaInfo;
}


/*! Assignment operator. Assigns the contents of the other object to this one.
 *
 * \param other the other snapshot object
 */
Snapshot &Snapshot::operator= (const Snapshot &other)
{
    m_execution = other.m_execution;
    m_origin = other.m_origin;
    m_location = other.m_location;
    m_name = other.m_name;
    m_metaInfo = other.m_metaInfo;

    return *this;
}


/*! Reset all internal data structures. The object is now in the same state,
 *  as if it has been just created.
 */
void Snapshot::reset()
{
    m_execution = QDateTime::currentDateTime();
    m_origin.clear();
    m_location.clear();
    m_name.clear();
    m_metaInfo.reset();
}


/*! \return the modification time of the snapshot
 */
QDateTime Snapshot::modificationTime() const
{
    return m_execution;
}


void  Snapshot::setModificationTime(const QDateTime &modificationTime)
{
    m_execution = modificationTime;
}


/*! \return the origin (location on physical storage) of the snapshot
 */
QString Snapshot::origin() const
{
    return m_origin;
}


/*! Set the origin (location on physical storage) of the snapshot
 *
 * \param origin the origin to be set to the snapshot
 */
void  Snapshot::setOrigin(const QString &origin)
{
    m_origin = origin;
}


/*! \return the location (logical location, can me mapped through encryption) of the snapshot
 */
QString Snapshot::location() const
{
    return m_location;
}


/*! Set the location of the snapshot
 *
 * \param location the location to be set to the snapshot
 */
void Snapshot::setLocation(const QString &location)
{
    m_location = location;
}


/*! \return the name of the snapshot. This is usually the directory name of the snapshot on the drive.
 */
QString Snapshot::name() const
{
    return m_name;
}


/*! Set the name of the snapshot
 *
 * \param name the name to be set to the snapshot
 */
void  Snapshot::setName(const QString &name)
{
    m_name = name;
}


/*! \return the meta info of the snapshot (see SnapshotMetaInfo for details)
 */
SnapshotMetaInfo Snapshot::metaInfo() const
{
    return m_metaInfo;
}


/*! Set the meta info of the snapshot
 *
 * \param metainfo the metainfo to be set to the snapshot
 */
void Snapshot::setMetaInfo(const SnapshotMetaInfo &metainfo)
{
    m_metaInfo = metainfo;
}

