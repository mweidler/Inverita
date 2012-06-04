/*
 * Utilities.cpp
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


#include <QtGlobal>
#include <QString>
#include <QDir>

#include "ApplicationException.h"

#ifdef Q_OS_UNIX
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#else
#error "will not compile"
#endif

/*
 * Syncs the meta file data from reference to target
 *
 * On success, zero is returned.
 * On error, -1 is returned, and errno is set appropriately.
 */
int CopyMeta(QString &reference, QString &target)
{
#ifdef Q_OS_UNIX
    struct stat meta;
    struct timespec times[2];

    if (lstat(reference.toStdString().c_str(), &meta) == -1) {
        return -1;
    }

    times[0].tv_sec = meta.st_atime;
    times[0].tv_nsec = 0;
    times[1].tv_sec = meta.st_mtime;
    times[1].tv_nsec = 0;
    // If pathname is absolute, then dirfd is ignored.
    if (utimensat(/*dirfd*/0, target.toStdString().c_str(), times, AT_SYMLINK_NOFOLLOW) == -1) {
        return -1;
    }

    // Do not chmod() on symbolic links, they have all permissions.
    if (!S_ISLNK(meta.st_mode)) {
        if (chmod(target.toStdString().c_str(), meta.st_mode) == -1) {
            return -1;
        }
    }

    if (lchown(target.toStdString().c_str(), meta.st_uid, meta.st_uid) == -1) {
        return -1;
    }

    return 0;
#endif
}


/*
 * Returns the content of the symbolic link 'filepath'.
 * In contrast to QFileInfo::symLinkTarget(), it does not return
 * the absolute path to the file or directory a symlink points to.
 */
QString ReadLink(const QString &filepath)
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
