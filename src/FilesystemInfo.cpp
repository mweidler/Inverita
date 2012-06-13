/**
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

FilesystemInfo::FilesystemInfo()
{
}

FilesystemInfo::FilesystemInfo(const QString &file)
{
    setFile(file);
}

void FilesystemInfo::setFile(const QString &file)
{
    m_absolutePath = file;
    emit changed();
}

qreal FilesystemInfo::usedCapacity()
{
    readStat();

    qreal totalSize = m_st.f_bsize * m_st.f_blocks;
    qreal available = m_st.f_bsize * m_st.f_bavail;

    qDebug() << totalSize << available << 1.0 - (totalSize / available);
    return 1.0 - (totalSize / available);
}

void FilesystemInfo::readStat()
{
    int rc;

    rc = statfs64(m_absolutePath.toStdString().c_str(), &m_st);
}
