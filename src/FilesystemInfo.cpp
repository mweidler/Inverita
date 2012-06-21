/*
 * FilesystemInfo.cpp
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


#include "FilesystemInfo.h"

#include <QDebug>
#include <QThread>


/*! Constructs a new object providing infos of the root file system.
 *
 *  From now on, the file system information is based on the root file system.
 */
FilesystemInfo::FilesystemInfo()
{
    setFile(".");
}


/*! Constructs a new object providing infos of a given file system containing the reference file.
 *
 *  From now on, the file system information is based on the file system
 *  where the reference file is located on.
 *
 *  \param file reference file
 */
FilesystemInfo::FilesystemInfo(const QString &file)
{
    setFile(file);
}


/*! Set a new reference file. From now on, the file system information
 *  is based on the file system where the reference file is located on.
 *
 *  \param file reference file
 */
void FilesystemInfo::setFile(const QString &file)
{
    m_absolutePath = file;
    refresh();
}


/*! Reads actual information from the file system.
 */
void FilesystemInfo::refresh()
{
    qDebug() << "FilesystemInfo::refresh()" << QThread::currentThreadId();

    if (statfs(m_absolutePath.toUtf8().data(), &m_st) == 0) {
        emit dataChanged();
    } else {
        memset(&m_st, 0, sizeof(m_st));
    }
}


/*! Returns the capacity of the file system.
 *  The value is between 0 (no capacity, full) and 1 (full capacity, empty).
 *
 * \return capacity of the file system
 */
qreal FilesystemInfo::capacity()
{
    if (totalCapacity() == 0) {
        return 0.0;
    }

    return 1.0 - ((qreal)usedCapacity() / totalCapacity());
}


/*! \return the number of used bytes in the file system.
 */
qint64 FilesystemInfo::usedCapacity()
{
    return (totalCapacity() - freeCapacity());
}


/*! \return the number of free/available bytes in the file system.
 */
qint64 FilesystemInfo::freeCapacity()
{
    return ((qint64)m_st.f_bsize) * m_st.f_bavail;
}


/*! \return the total number (size) bytes in the file system.
 */
qint64 FilesystemInfo::totalCapacity()
{
    return ((qint64)m_st.f_bsize) * m_st.f_blocks;
}

