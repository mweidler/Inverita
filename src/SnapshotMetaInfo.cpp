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


#include "SnapshotMetaInfo.h"
#include <QDebug>
#include <QFile>
#include <QSettings>


SnapshotMetaInfo::SnapshotMetaInfo()
{
    reset();
}

SnapshotMetaInfo::SnapshotMetaInfo(const SnapshotMetaInfo &other) : QObject()
{
    m_files = other.m_files;
    m_totalSize = other.m_totalSize;
    m_checksum = other.m_checksum;
    m_isValid = other.m_isValid;
}

SnapshotMetaInfo &SnapshotMetaInfo::operator= (const SnapshotMetaInfo &other)
{
    m_files = other.m_files;
    m_totalSize = other.m_totalSize;
    m_checksum = other.m_checksum;
    m_isValid = other.m_isValid;

    return *this;
}

void SnapshotMetaInfo::reset()
{
    m_files = 0;
    m_totalSize = 0;
    m_checksum.clear();
    m_isValid = false;
}

bool SnapshotMetaInfo::load(QString filename)
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
    m_checksum = settings.value("Checksum").toString().toAscii();
    m_isValid = settings.value("Valid", 0).toBool();

    if (m_files == 0) {
        return false;
    }

    return true;
}


void SnapshotMetaInfo::save(QString filename)
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
    settings.setValue("Checksum",  QString(m_checksum));
    settings.setValue("Valid",  m_isValid);

    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save settings '" + filename + "'");
        e.setErrorMessage(tr("Error during writing"));
        throw e;
    }
}

QByteArray SnapshotMetaInfo::checksum() const
{
    return m_checksum;
}

void SnapshotMetaInfo::setChecksum(const QByteArray &checksum)
{
    m_checksum = checksum;
}

qint64 SnapshotMetaInfo::numberOfFiles() const
{
    return m_files;
}

void SnapshotMetaInfo::setNumberOfFiles(qint64 count)
{
    m_files = count;
}

qint64 SnapshotMetaInfo::sizeOfFiles() const
{
    return m_totalSize;
}

void SnapshotMetaInfo::setSizeOfFiles(qint64 count)
{
    m_totalSize = count;
}

bool SnapshotMetaInfo::isValid() const
{
    return m_isValid;
}

void SnapshotMetaInfo::setValid(bool valid)
{
    m_isValid = valid;
}
