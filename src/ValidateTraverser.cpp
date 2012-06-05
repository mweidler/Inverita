/**
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
#include "sha1.h"


/*! Default constructor
 */
ValidateTraverser::ValidateTraverser()
{
}


/*! \returns Returns the signatures map.
 *
 */
SignatureMap &ValidateTraverser::signatures()
{
    return m_signatures;
}


/*! Set the current backup path
 *
 *  \param path the absolute path to the current backup path
 */
void ValidateTraverser::setBackupPath(QString &path)
{
    m_backupPath = path;
    m_sizeOfBackupPath = path.length();
    qDebug() << "Validate set backup path"  << m_backupPath << m_sizeOfBackupPath;
}


/*! Create a hash code for the file.
 *
 *  The number of processed bytes are added to byte counter.
 *  Hashing can be aborted by flagging.
 *
 *  \param  sourcefilename the absolute path to the file to be copied
 *  \param  hash           hash signature of the file content
 *  \return true on success, otherwise false
 */
bool ValidateTraverser::hashFile(const QString &sourcefilename, QByteArray &hash)
{
    qint64 bytesRead;
    QByteArray hashoutput(20, '\0');
    sha1_context ctx;
    bool   success;

    QFile source(sourcefilename);
    success = source.open(QIODevice::ReadOnly);
    if (!success) {
        return false;
    }

    sha1_starts(&ctx);

    do {
        bytesRead = source.read(m_fileBuffer, sizeof(m_fileBuffer));

        sha1_update(&ctx, (const unsigned char *)m_fileBuffer, bytesRead);
        m_totalSize += bytesRead;

    } while (bytesRead == (qint64)sizeof(m_fileBuffer) && !m_abort);

    sha1_finish(&ctx, (unsigned char *)hashoutput.data());
    hash = hashoutput.toHex();
    memset(&ctx, 0, sizeof(sha1_context));

    return true;
}


/*! Is called during directory traversal if a file was found.
 *
 *  \param absoluteFilePath the absolute path to the found file
 */
void ValidateTraverser::onFile(const QString &absoluteFilePath)
{
    QByteArray previousHash;
    QByteArray currentHash;
    QString    key = absoluteFilePath.mid(m_sizeOfBackupPath);

    qDebug() << "Validate onFile" << absoluteFilePath << key;

    previousHash = m_signatures.value(key);
    hashFile(absoluteFilePath, currentHash);

    if (previousHash != currentHash) {
        qDebug() << absoluteFilePath << "has beed modified";
    }
    else {
        qDebug() << absoluteFilePath << "is ok";
    }

    m_totalFiles++;
}


/*! Is called during directory traversal if a new directory is entered.
 *
 *  \param absoluteFilePath the absolute path to the new directory
 */
void ValidateTraverser::onEnterDir(const QString &/*absoluteFilePath*/)
{
    // do nothing
}


/*! Is called during directory traversal if a directory is left.
 *
 *  \param absoluteFilePath the absolute path to the left directory
 */
void ValidateTraverser::onLeaveDir(const QString &/*absoluteFilePath*/)
{
    // do nothing
}


/*! Is called during directory traversal if a symbolic link was found.
 *
 *  \code
 *  Example:
 *  lrwxrwxrwx 2 user users    4 May 10 14:13 linktodir1 -> dir1
 *           'linktodir1' is the absoluteFilePath, 'dir1' is the linkname
 *  \endcode
 *
 *  \param absoluteFilePath the absolute path to the link
 *  \param linkName         the name of the link
 */
void ValidateTraverser::onLink(const QString &/*absoluteFilePath*/, const QString &/*linkName*/)
{
    // do nothing
}


/*! Is called during directory traversal if a other item (no link, no file, no directory) was found.
 *
 *  \param absoluteFilePath the absolute path to the file system item
 */
void ValidateTraverser::onOther(const QString &/*absoluteFilePath*/)
{
    // do nothing
}
