/**
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
#include "sha1.h"

#ifdef Q_OS_UNIX
#include <errno.h>
#endif


/*! Default constructor
 */
CopyTraverser::CopyTraverser()
{
}


/*! \returns Returns the signatures map.
 *
 */
SignatureMap &CopyTraverser::currentSignatures()
{
    return m_currentSignatures;
}

SignatureMap &CopyTraverser::previousSignatures()
{
    return m_previousSignatures;
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
bool CopyTraverser::equalsFile(QString &newfilename, QString &reffilename)
{
    QFileInfo refInfo(reffilename);
    QFileInfo newInfo(newfilename);

    qDebug() << "new file:"  << newInfo.lastModified() <<
             "reference:"  << refInfo.lastModified();

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


/*! Copy file system items (file, link, dir, other) from one place to
 *  an other while using hard links for already existing files on the other place.
 *
 *  The number of copied bytes are added to byte counter. Copy can be aborted by flagging.
 *
 *  \param  sourcefilename the absolute path to the file to be copied
 *  \param  targetfilename the absolute path to the location to be copied
 *  \param  hash           hash signature of the file content
 *  \return true on success, otherwise false
 */
bool CopyTraverser::copyFile(QString &sourcefilename, QString &targetfilename, QByteArray &hash)
{
    qint64 bytesRead, bytesWritten;
    QByteArray hashoutput(20, '\0');
    sha1_context ctx;
    bool   success;

    QFile::remove(targetfilename);

    QFile source(sourcefilename);
    success = source.open(QIODevice::ReadOnly);
    if (!success) {
        return false;
    }

    sha1_starts(&ctx);
    QFile target(targetfilename);
    success = target.open(QIODevice::WriteOnly);
    if (success) {
        do {
            bytesRead = source.read(m_copyBuffer, sizeof(m_copyBuffer));
            bytesWritten = target.write(m_copyBuffer, bytesRead);

            sha1_update(&ctx, (const unsigned char *)m_copyBuffer, bytesRead);
            m_totalSize += bytesWritten;

            // e.g. disk full error
            if (bytesRead != bytesWritten) {
                return false;
            }
        } while (bytesRead == (qint64)sizeof(m_copyBuffer) && !m_abort);
    }

    sha1_finish(&ctx, (unsigned char *)hashoutput.data());
    hash = hashoutput.toHex();
    memset(&ctx, 0, sizeof(sha1_context));

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
    QByteArray hash;

    qDebug() << "onFile" << source << target << previous;

    /* If a previous backup already exists and there is already exactly the same file,
     * we create a hard link on this file instead of creating/copying a new file.
     */
    if (m_previousBackupPath.size() && equalsFile(source, previous)) {
        qDebug() << "linking " << previous << target;

        int rc = link(previous.toStdString().c_str(), target.toStdString().c_str());
        if (rc == -1) {
            ApplicationException e;
            e.setCauser("create hard link from '" + previous + "' to '" + target + "'");
            e.setErrorMessage(strerror(errno));
            throw e;
        }

        // take hash from previous backup
        hash = m_previousSignatures.value(source);
        qDebug() << "link sig:" << source << hash;
        m_currentSignatures.insert(source, hash);

        QFile target(source);
        m_totalSize += target.size();

    } else {
        qDebug() << "copy" << source << target;

        bool success = copyFile(source, target, hash);
        if (!success) {
            ApplicationException e;
            e.setCauser(tr("Copy error from") + "'" + source + "' to '" + target + "'");
            e.setErrorMessage(tr("Copy error"));
            throw e;
        }

        // hash has been generated during copy
        m_currentSignatures.insert(source, hash);
        qDebug() << "Hash:" << hash;

        int rc = CopyMeta(source, target);
        if (rc == -1) {
            ApplicationException e;
            e.setCauser("Set file meta data  '" + source + "' to '" + target + "'");
            e.setErrorMessage(strerror(errno));
            throw e;
        }
    }

    m_totalFiles++;
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
        e.setCauser("Create directory '" + fullpath + "'");
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
        e.setCauser("Set folder meta data  '" + source + "' to '" + target + "'");
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
    int rc = symlink(linkName.toStdString().c_str(), source.toStdString().c_str());
    if (rc == -1) {
        ApplicationException e;
        e.setCauser("Create symbolic link '" + source + "' to '" + linkName + "'");
        e.setErrorMessage(strerror(errno));
        throw e;
    }

    QString source1 = absoluteFilePath;
    QString target1 = m_currentBackupPath + absoluteFilePath;
    qDebug() << "LinkSync:" << source1 << target1;

    rc = CopyMeta(source1, target1);
    if (rc == -1) {
        ApplicationException e;
        e.setCauser("Set file meta data  '" + source1 + "' to '" + target1 + "'");
        e.setErrorMessage(strerror(errno));
        throw e;
    }
}


/*! Is called during directory traversal if a other item (no link, no file, no directory) was found.
 *
 *  \param absoluteFilePath the absolute path to the file system item
 */
void CopyTraverser::onOther(const QString &absoluteFilePath)
{
    // do nothing
    qDebug() << "Mirror: other:" << absoluteFilePath;
}
