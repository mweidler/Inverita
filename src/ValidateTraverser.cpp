/*
 * ValidateTraverser.cpp
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


#include "ValidateTraverser.h"
#include "Checksum.h"
#include "Backup.h"

#include <QDebug>


/*! Default constructor
 */
ValidateTraverser::ValidateTraverser()
{
}


/*! \returns Returns the digests map.
 *
 */
DigestsMap &ValidateTraverser::digests()
{
    return m_digests;
}


/*! Set the current backup path
 *
 *  \param path the absolute path to the current backup path
 */
void ValidateTraverser::setBackupPath(const QString &path)
{
    m_backupPath = path;
    m_sizeOfBackupPath = path.length();
    qDebug() << "Validate set backup path"  << m_backupPath << m_sizeOfBackupPath;
}


void ValidateTraverser::evaluate(SnapshotMetaInfo &metaInfo)
{
    if (metaInfo.numberOfFiles() != files()) {
        emit report(tr("%1 files expected, but %2 files found.").arg(metaInfo.numberOfFiles()).arg(files()) + "<br>");
        countError();
    }

    if (metaInfo.sizeOfFiles() != processed()) {
        emit report(tr("%1 bytes expected, but %2 bytes found.<br>").arg(metaInfo.sizeOfFiles()).arg(processed()) + "<br>");
        countError();
    }

    QList<QString> keys = m_digests.keys();
    if (keys.size() > 0) {
        report(tr("The following files are missing in the backup snapshot:") + "<br>");
        for (int i = 0; i < keys.size(); i++)  {
            emit report(keys[i] + "<br>");
            countFile();
        }
        emit report("<br>");
    }

    if (Backup::instance().config().verifyDigest() == false) {
        emit report(tr("WARNING: Content digests not verified (disabled).") + "<br>");
    }

    if (errors() > 0) {
        metaInfo.setQuality(SnapshotMetaInfo::Unknown);
        emit report(tr("Backup snapshot invalidated.") + "<br><br>");
    } else {
        metaInfo.setQuality(SnapshotMetaInfo::Reliable);
        emit report(tr("Backup snapshot is valid.") + "<br><br>");
    }
}


/*! Create a digest of the file content.
 *
 *  The number of processed bytes are added to byte counter.
 *  Hashing can be aborted by flagging.
 *
 *  \param  sourcefilename the absolute path to the file to be copied
  *  \return Computed hash digest of the file content
 */
QByteArray ValidateTraverser::computeDigestOfFile(const QString &sourcefilename)
{
    Checksum checksum;
    qint64 bytesRead;
    bool success;

    QFile source(sourcefilename);
    success = source.open(QIODevice::ReadOnly);
    if (!success) {
        return QByteArray();
    }

    do {
        bytesRead = source.read(m_fileBuffer, sizeof(m_fileBuffer));

        checksum.update(m_fileBuffer, bytesRead);
        countProcessed(bytesRead);
        countTransferred(bytesRead);

    } while (bytesRead == (qint64)sizeof(m_fileBuffer) && !shouldAbort());

    return checksum.finish();
}


/*! Is called during directory traversal if a file was found.
 *
 *  \param absoluteFilePath the absolute path to the found file
 */
void ValidateTraverser::onFile(const QString &absoluteFilePath)
{
    QString key = absoluteFilePath.mid(m_sizeOfBackupPath);

    // TODO: rename all Hash naming to Digest
    if (Backup::instance().config().verifyDigest()) {
        QByteArray previousDigest = m_digests.value(key);
        QByteArray currentDigest = computeDigestOfFile(absoluteFilePath);

        if (previousDigest != currentDigest) {
            emit report(tr("'%1' has beed modified.").arg(absoluteFilePath) + "<br>");
            countError();
        }
    } else {
        QFile file(absoluteFilePath);
        countProcessed(file.size());
    }

    m_digests.remove(key);
    countFile();
}
