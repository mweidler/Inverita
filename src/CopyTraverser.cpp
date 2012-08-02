/*
 * CopyTraverser.cpp
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


#include "CopyTraverser.h"
#include "Utilities.h"
#include "Checksum.h"

#include <QDateTime>
#include <QDir>

#ifdef Q_OS_UNIX
#include <errno.h>
#endif


/*! Default constructor
 */
CopyTraverser::CopyTraverser()
{
}


/*! \returns Returns the digests map.
 */
DigestsMap &CopyTraverser::currentDigests()
{
    return m_currentDigests;
}

DigestsMap &CopyTraverser::previousDigests()
{
    return m_previousDigests;
}

/*! Set the previous backup path
 *
 *  \param path the absolute path to the previous backup path
 */
void CopyTraverser::setPreviousBackupPath(QString &path)
{
    m_previousBackupPath = path;
}


/*! Set the current backup path
 *
 *  \param path the absolute path to the current backup path
 */
void CopyTraverser::setCurrentBackupPath(QString &path)
{
    m_currentBackupPath = path;
}


/*! Investigates, if a already existing file (reference) is equal to
 *  to the new (to be copied) file. The file must have the same name and
 *  meta attributes.
 *
 *  \param  newfilename  the absolute path to the file to be copied
 *  \param  reffilename  the absolute path to the file compared to
 *  \return true if file is equal, otherwise false
 */
bool CopyTraverser::compareFiles(QString &newfilename, QString &reffilename)
{
    QFileInfo refInfo(reffilename);
    QFileInfo newInfo(newfilename);

    if (newInfo.lastModified() != refInfo.lastModified()) {
        return false;
    }

    if (newInfo.groupId() != refInfo.groupId()) {
        return false;
    }

    if (newInfo.ownerId() != refInfo.ownerId()) {
        return false;
    }

    if (newInfo.size() != refInfo.size()) {
        return false;
    }

    return true;
}


/*! Copy file system items (file, link, dir, other) from one place to an other.
 *
 *  The number of copied bytes are added to byte counter. Copy can be aborted by flagging.
 *
 *  \param  sourcefilename the absolute path to the file to be copied
 *  \param  targetfilename the absolute path to the location to be copied
 *  \param  digest         digest of the file content
 *  \return true on success, otherwise false
 */
bool CopyTraverser::copyFile(QString &sourcefilename, QString &targetfilename, QByteArray &digest)
{
    Checksum checksum;
    qint64 bytesRead, bytesWritten;
    bool success;

    QFile source(sourcefilename);
    success = source.open(QIODevice::ReadOnly);
    if (!success) {
        return false;
    }

    QFile target(targetfilename);
    success = target.open(QIODevice::WriteOnly);
    if (success) {
        do {
            bytesRead = source.read(m_copyBuffer, sizeof(m_copyBuffer));
            bytesWritten = target.write(m_copyBuffer, bytesRead);

            checksum.update(m_copyBuffer, bytesRead);
            countProcessed(bytesWritten);
            countTransferred(bytesWritten);

            // e.g. disk full error
            if (bytesRead != bytesWritten) {
                target.close();
                QFile::remove(targetfilename);
                return false;
            }
        } while (bytesRead == (qint64)sizeof(m_copyBuffer) && !shouldAbort());
    }

    digest = checksum.finish();
    return true;
}


/*! Is called during directory traversal if a file was found.
 *
 *  \param absoluteFilePath the absolute path to the found file
 */
void CopyTraverser::onFile(const QString &absoluteFilePath)
{
    QString source = absoluteFilePath;
    QString target = m_currentBackupPath + absoluteFilePath;
    QString previous = m_previousBackupPath + absoluteFilePath;
    QByteArray digest;

    /* If a previous backup already exists and there is already exactly the same file,
     * we create a hard link on this file instead of creating/copying a new file.
     */
    if (m_previousBackupPath.size() && compareFiles(source, previous)) {
        int rc = link(previous.toUtf8().data(), target.toUtf8().data());
        if (rc == -1) {
            ApplicationException e;
            e.setCauser(tr("Create hard link from '%1' to '%2'").arg(previous).arg(target));
            e.setErrorMessage(strerror(errno));
            throw e;
        }

        // take digest from previous backup
        digest = m_previousDigests.value(source);
        m_currentDigests.insert(source, digest);

        QFile target(source);
        countProcessed(target.size());

    } else {
        bool success = copyFile(source, target, digest);
        if (!success) {
            ApplicationException e;
            e.setCauser(tr("Copy file from '%1' to '%2'").arg(source).arg(target));
            e.setErrorMessage(tr("Copy error, disk full?"));
            throw e;
        }

        // digest has been generated during copy
        m_currentDigests.insert(source, digest);

        int rc = CopyMeta(source, target);
        if (rc == -1) {
            ApplicationException e;
            e.setCauser(tr("Set file meta data  '%1' to '%2'").arg(source).arg(target));
            e.setErrorMessage(strerror(errno));
            throw e;
        }
    }

    countFile();
}


/*! Is called during directory traversal if a new directory is entered.
 *
 *  \param absoluteFilePath the absolute path to the new directory
 */
void CopyTraverser::onEnterDir(const QString &absoluteFilePath)
{
    QString fullpath = m_currentBackupPath + absoluteFilePath;

    QDir dir;
    bool success = dir.mkpath(fullpath);
    if (!success) {
        ApplicationException e;
        e.setCauser(tr("Create directory '%1'").arg(fullpath));
        e.setErrorMessage(tr("Could not create directory"));
        throw e;
    }
}


/*! Is called during directory traversal if a directory is left.
 *
 *  \param absoluteFilePath the absolute path to the left directory
 */
void CopyTraverser::onLeaveDir(const QString &absoluteFilePath)
{
    QString source = absoluteFilePath;
    QString target = m_currentBackupPath + absoluteFilePath;

    int rc = CopyMeta(source, target);
    if (rc == -1) {
        ApplicationException e;
        e.setCauser(tr("Set folder meta data  '%1' to '%2'").arg(source).arg(target));
        e.setErrorMessage(strerror(errno));
        throw e;
    }
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
void CopyTraverser::onLink(const QString &absoluteFilePath, const QString &linkName)
{
    QString source = m_currentBackupPath + absoluteFilePath;
    int rc = symlink(linkName.toUtf8().data(), source.toUtf8().data());

    // A symlink is not documented intuitively, here is an example:
    // e.g. symlink("abc/testfile.txt", "/home/user/xfile");
    //
    //      $ ls /home/user
    //      lrwxrwxrwx 1 user users 13 May  9 07:40 xfile -> abc/testfile.txt

    if (rc == -1) {
        ApplicationException e;
        e.setCauser(tr("Create symbolic link '%1' to '%2'").arg(source).arg(linkName));
        e.setErrorMessage(strerror(errno));
        throw e;
    }

    QString source1 = absoluteFilePath;
    QString target1 = m_currentBackupPath + absoluteFilePath;

    rc = CopyMeta(source1, target1);
    if (rc == -1) {
        ApplicationException e;
        e.setCauser(tr("Copy file meta data from '%1' to '%2'").arg(source1).arg(target1));
        e.setErrorMessage(strerror(errno));
        throw e;
    }
}
