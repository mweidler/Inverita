/*
 * SnapshotMetaInfo.cpp
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

#include "SnapshotMetaInfo.h"


SnapshotMetaInfo::SnapshotMetaInfo()
{
    reset();
}

void SnapshotMetaInfo::reset()
{
    m_files = 0;
    m_totalSize = 0;
}

bool SnapshotMetaInfo::Load(QString filename)
{
    reset();

    if (QFile::exists(filename) == false) {
        return false;
    }

    QSettings settings(filename,  QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("load meta info '" + filename + "'");
        e.setErrorMessage(tr("Can not be opened"));
        throw e;
    }

    m_files = (qint64)settings.value("TotalFiles", 0).toLongLong();
    m_totalSize = (qint64)settings.value("TotalSize", 0).toLongLong();

    if (m_files == 0)
        return false;

    return true;
}


void SnapshotMetaInfo::Save(QString filename)
{
    QSettings settings(filename,  QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save meta info '" + filename + "'");
        e.setErrorMessage(tr("Can not be created"));
        throw e;
    }

    settings.setValue("TotalFiles",  m_files);
    settings.setValue("TotalSize",  m_totalSize);

    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save settings '" + filename + "'");
        e.setErrorMessage(tr("Error during writing"));
        throw e;
    }
}

qint64 SnapshotMetaInfo::numberOfFiles()
{
    return m_files;
}

void SnapshotMetaInfo::setNumberOfFiles(qint64 count)
{
    m_files = count;
}

qint64 SnapshotMetaInfo::sizeOfFiles()
{
    return m_totalSize;
}

void SnapshotMetaInfo::setSizeOfFiles(qint64 count)
{
    m_totalSize = count;
}
