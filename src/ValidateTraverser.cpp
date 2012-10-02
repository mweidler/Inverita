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
#include "IOBuffer.h"


/*! Default constructor
 */
ValidateTraverser::ValidateTraverser()
{
    m_sizeOfBackupPath = 0;
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
}


void ValidateTraverser::evaluate(SnapshotMetaInfo &metaInfo)
{
    if (metaInfo.fileCount() != files()) {
        emit report(tr("%1 files expected, but %2 files found.").arg(metaInfo.fileCount()).arg(files()) + "<br>");
        countError();
    }

    if (metaInfo.dataSize() != processed()) {
        emit report(tr("%1 Bytes expected, but %2 Bytes found.").arg(metaInfo.dataSize()).arg(processed()) + "<br>");
        countError();
    }

    QList<QString> keys = m_digests.keys();
    if (keys.size() > 0) {
        report(tr("The following files are missing in the backup snapshot:") + "<br>");
        for (int i = 0; i < keys.size(); i++)  {
            if (i < 25) {
                emit report(keys[i] + "<br>");
            } else if (i == 25) {
                emit report(tr("More files are missing...") + "<br>");
            }

            countFile();
        }
        emit report("<br>");
    }

    if (Backup::instance().config().verifyDigest() == false) {
        emit report(tr("WARNING: Content digests not verified (disabled).") + "<br>");
    }

    if (errors() > 0) {
        metaInfo.setQuality(SnapshotMetaInfo::Unknown);
        emit report(tr("Backup snapshot status set to 'Unknown'.") + "<br><br>");
    } else {
        metaInfo.setQuality(SnapshotMetaInfo::Reliable);
        emit report(tr("All files validated, Backup snapshot is reliable.") + "<br><br>");
    }
}


/*! Create a digest of the file content.
 *
 *  The number of processed bytes are added to byte counter.
 *  Digest computing can be aborted by flagging.
 *
 *  \param  sourcefilename the absolute path to the file to be copied
  *  \return Computed digest of the file content
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

    IOBuffer &iobuffer = IOBuffer::instance();

    do {
        iobuffer.begin();
        bytesRead = source.read(iobuffer.buffer().data(), iobuffer.chunkSize());

        checksum.update(iobuffer.buffer(), bytesRead);
        iobuffer.finished();

        countProcessed(bytesRead);
        countTransferred(bytesRead);

    } while (bytesRead == iobuffer.chunkSize() && !shouldAbort());

    return checksum.finish();
}


/*! Is called during directory traversal if a file was found.
 *
 *  \param absoluteFilePath the absolute path to the found file
 */
void ValidateTraverser::onFile(const QString &absoluteFilePath)
{
    QString key = "." + absoluteFilePath.mid(m_sizeOfBackupPath);

    if (Backup::instance().config().verifyDigest()) {
        QByteArray previousDigest = m_digests.value(key);
        QByteArray currentDigest = computeDigestOfFile(absoluteFilePath);

        if (previousDigest != currentDigest) {
            if (errors() < 25) {
                emit report(tr("File '%1' has beed modified.").arg(absoluteFilePath) + "<br>");
            } else if (errors() == 25) {
                emit report(tr("More files have been modified...") + "<br>");
            }

            countError();
        }
    } else {
        QFile file(absoluteFilePath);
        countProcessed(file.size());
    }

    m_digests.remove(key);
    countFile();
}
