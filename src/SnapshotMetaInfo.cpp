/*
 * SnapshotMetaInfo.cpp
 *
 * Copyright (c) 2012, Marc Weidler <marc.weidler@web.de>
 * Ulrichstr. 12/1, 71672 Marbach, Germany
 * All rights reserved.
 *
 * 'ALVARA Personal Backup' is licensed under the BSD license (simplified, 2-clause).
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
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

void SnapshotMetaInfo::Load(QString filename)
{
    reset();

    QSettings settings(filename,  QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("load meta info '" + filename + "'");
        e.setErrorMessage(tr("Can not be opened"));
        throw e;
    }

    m_files = (qint64)settings.value("TotalFiles", 0).toLongLong();
    m_totalSize = (qint64)settings.value("TotalSize", 0).toLongLong();
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
