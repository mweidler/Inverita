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
    m_filecount = other.m_filecount;
    m_dataSize = other.m_dataSize;
    m_checksum = other.m_checksum;
    m_quality = other.m_quality;
}

SnapshotMetaInfo &SnapshotMetaInfo::operator= (const SnapshotMetaInfo &other)
{
    m_filecount = other.m_filecount;
    m_dataSize = other.m_dataSize;
    m_checksum = other.m_checksum;
    m_quality = other.m_quality;

    return *this;
}

void SnapshotMetaInfo::reset()
{
    m_filecount = 0;
    m_dataSize = 0;
    m_checksum.clear();
    m_quality = Unknown;
}

bool SnapshotMetaInfo::load(const QString &filename)
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

    m_filecount = (qint64)settings.value("filecount", 0).toLongLong();
    m_dataSize = (qint64)settings.value("datasize", 0).toLongLong();
    m_checksum = settings.value("checksum").toString().toAscii();
    QString quality = settings.value("quality", "unknown").toString();
    if (quality == "unknown") {
        m_quality = Unknown;
    }
    if (quality == "partial") {
        m_quality = Partial;
    }
    if (quality == "complete") {
        m_quality = Complete;
    }
    if (quality == "reliable") {
        m_quality = Reliable;
    }

    return true;
}


void SnapshotMetaInfo::save(const QString &filename)
{
    QSettings settings(filename,  QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save meta info '" + filename + "'");
        e.setErrorMessage(tr("Can not be created"));
        throw e;
    }

    settings.setValue("filecount",  m_filecount);
    settings.setValue("datasize",  m_dataSize);
    settings.setValue("checksum",  QString(m_checksum));
    switch (m_quality) {
        case Unknown:
            settings.setValue("quality", QString("unknown"));
            break;
        case Partial:
            settings.setValue("quality", QString("partial"));
            break;
        case Complete:
            settings.setValue("quality", QString("complete"));
            break;
        case Reliable:
            settings.setValue("quality", QString("reliable"));
            break;
        default:
            break;
    }

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

qint64 SnapshotMetaInfo::fileCount() const
{
    return m_filecount;
}

void SnapshotMetaInfo::setFileCount(qint64 count)
{
    m_filecount = count;
}

qint64 SnapshotMetaInfo::dataSize() const
{
    return m_dataSize;
}

void SnapshotMetaInfo::setDataSize(qint64 count)
{
    m_dataSize = count;
}

SnapshotMetaInfo::Quality SnapshotMetaInfo::quality() const
{
    return m_quality;
}

void SnapshotMetaInfo::setQuality(Quality quality)
{
    m_quality = quality;
}
