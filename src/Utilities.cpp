/*
 * Utilities.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2014  Marc Weidler <marc.weidler@web.de>,
 *                          Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                          All rights reserved.
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


#include "Utilities.h"

#include <QtGlobal>
#include <QString>
#include <QDir>
#include <QLocale>

#include "ApplicationException.h"

#ifdef Q_OS_UNIX
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#else
#error "will not compile"
#endif


/*! Syncs the meta file data from reference to target
 *
 *  On success, zero is returned.
 *  On error, -1 is returned, and errno is set appropriately.
 */
int CopyMeta(QString &reference, QString &target)
{
#ifdef Q_OS_UNIX
    struct stat meta;
    struct timespec times[2];

    if (lstat(reference.toUtf8().data(), &meta) == -1) {
        return -1;
    }

    times[0].tv_sec = meta.st_atime;
    times[0].tv_nsec = 0;
    times[1].tv_sec = meta.st_mtime;
    times[1].tv_nsec = 0;
    // If pathname is absolute, then dirfd is ignored.
    if (utimensat(/*dirfd*/0, target.toUtf8().data(), times, AT_SYMLINK_NOFOLLOW) == -1) {
        return -1;
    }

    // Do not chmod() on symbolic links, they have all permissions.
    if (!S_ISLNK(meta.st_mode)) {
        if (chmod(target.toUtf8().data(), meta.st_mode) == -1) {
            return -1;
        }
    }

    if (lchown(target.toUtf8().data(), meta.st_uid, meta.st_uid) == -1) {
        return -1;
    }

    return 0;
#endif
}


/*! Returns the content of the symbolic link 'filepath'.
 *  In contrast to QFileInfo::symLinkTarget(), it does not return
 *  the absolute path to the file or directory a symlink points to.
 */
QString ReadSymbolicLink(const QString &filepath)
{
#ifdef Q_OS_UNIX
    struct stat sb;
    if (lstat(qPrintable(filepath), &sb) == -1) {
        ApplicationException e;
        e.setCauser("lstat '" + filepath + "'");
        e.setErrorMessage(strerror(errno));
        throw e;
    }

    QByteArray content(sb.st_size + 1, '\0');
    ssize_t r = readlink(qPrintable(filepath), content.data(), content.size());
    if (r == -1) {
        ApplicationException e;
        e.setCauser("readlink '" + filepath + "'");
        e.setErrorMessage(strerror(errno));
        throw e;
    }

    return content;
#endif
}


QString SearchLatestBackupDir(QString absolutePath)
{
    QDir dir(absolutePath);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);

    QFileInfoList list = dir.entryInfoList();
    if (list.size() == 0) {
        return "";
    } else {
        QFileInfo fileInfo = list.at(0);
        return fileInfo.absoluteFilePath();
    }
}


/*! Formats the size to a well human readable form.
 *
 *  This function uses decimal prefixes (SI) for base-10 units with multiple of 1000.
 *  See <http://wiki.ubuntu.com/UnitsPolicy> for details.
 *    1 Byte = 1 byte
 *    1 kB = 1,000 bytes (Note: small k)
 *    1 MB = 1,000 kByte = 1,000,000 bytes
 *    1 GB = 1,000 MByte = 1,000,000 kByte = 1,000,000,000 bytes
 *    1 TB = 1,000 GByte = 1,000,000 MByte = 1,000,000,000 kByte = 1,000,000,000,000 bytes
 *    1 PB = 1,000 TByte = 1,000,000 GByte = 1,000,000,000 MByte = 1,000,000,000,000 kByte = 1,000,000,000,000,000 bytes
 */
QString formatSize(qint64 size)
{
    int     unitIdx = 0;
    int     decimals = 0;
    qreal   value = size;
    QString stringValue;

    static QStringList units = (QStringList() << "Byte" << "kB" << "MB" << "GB" << "TB" << "PB");

    while (value >= 1000 && unitIdx < units.size()) {
        unitIdx++;
        value /= 1000;
    }

    if (value >= 100) {
        decimals = 0;
    } else if (value >= 10) {
        if (unitIdx > 0) {
            decimals = 1;
        }
    } else {
        if (unitIdx > 0) {
            decimals = 2;
        }
    }

    stringValue = QLocale().toString(value, 'f', decimals);

    return stringValue + " " + units[unitIdx];
}
