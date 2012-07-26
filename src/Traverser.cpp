/*
 * Traverser.cpp
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


#include "Traverser.h"
#include "Utilities.h"

#include <QDebug>
#include <QDir>


/*! Default constructor
 */
Traverser::Traverser()
{
    reset();
}


/*! Set all internals to it's default state.
 */
void Traverser::reset()
{
    m_totalFiles = 0;
    m_totalSize = 0;
    m_totalTransferred = 0;
    m_totalErrors = 0;
    m_abort = false;
    m_basePaths.clear();
    m_excludePatterns.clear();
}


/*! Add a specific directory (path) to the list to be traversed.
 */
void Traverser::addIncludes(QString &absolutePath)
{
    m_basePaths.append(absolutePath);
}


/*! Add a list of directories (paths) to the list to be traversed.
 */
void Traverser::addIncludes(QStringList &absolutePaths)
{
    m_basePaths.append(absolutePaths);
}


/*! Add a patterns to identify excludes.
 */
void Traverser::addExcludes(const char *absolutePath)
{
    m_excludePatterns.append(absolutePath);
}


/*! Add a patterns to identify excludes.
 */
void Traverser::addExcludes(QString &absolutePath)
{
    m_excludePatterns.append(absolutePath);
}


/*! Add a list of patterns to identify excludes.
 */
void  Traverser::addExcludes(QStringList &absolutePaths)
{
    m_excludePatterns.append(absolutePaths);
}

/*! \returns the number of traversed files.
 */
qint64 Traverser::totalFiles()
{
    return m_totalFiles;
}


/*! \returns the size of all traversed files.
 */
qint64 Traverser::totalSize()
{
    return m_totalSize;
}

/*! \returns the size of all transferred (read/write) files.
 */
qint64 Traverser::totalTransferred()
{
    return m_totalTransferred;
}


/*! \returns the number of occured errors.
 */
qint64 Traverser::totalErrors()
{
    return m_totalErrors;
}


/*! A call to this method signals the traverser, that
 *  the traversal should be aborted and returns immediately.
 *
 *  This method sets the member variable 'm_abort' to true.
 *  Traversal implementations must check this status as often
 *  as possible, and terminate it's execution.
 *
 *  Qt-hint: abort() can not be called via event loop (connect),
 *           because the worker thread blocks it's event queue.
 */
void Traverser::abort()
{
    qDebug() << "DirectoryTraverser: abort requested";
    m_abort = true;
}


/*! Start traversal over all given paths recusively and call
 *  the appropriate handler methods.
 */
void Traverser::traverse()
{
    for (int i = 0; i < m_basePaths.count() && !m_abort; i++) {
        QString currentBasePath = m_basePaths[i];

        onEnterDir(currentBasePath);
        recurseDirectory(currentBasePath);
        onLeaveDir(currentBasePath);
    }
}


bool Traverser::isExcluded(const QString &filepath)
{
    for (int i = 0; i < m_excludePatterns.count(); i++) {
        QRegExp rx(m_excludePatterns[i], Qt::CaseSensitive, QRegExp::Wildcard);
        if (filepath.contains(rx)) {
            return true;
        }
    }

    return false;
}


/*! Dispatches all found file system items to handler methods.
 *
 * \param dirname the directory name to be investigated
 */
void Traverser::recurseDirectory(const QString &dirname)
{
    QDir dir(dirname);
    if (!dir.exists()) {
        ApplicationException e;
        e.setCauser(tr("Access directory") + " '" + dirname + "'");
        e.setErrorMessage(tr("Directory not found"));
        throw e;
        return;
    }

    dir.setFilter(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList filelist = dir.entryInfoList();

    for (int i = 0; i < filelist.count() && !m_abort; i++) {
        QFileInfo fileinfo = filelist[i];
        QString filepath = fileinfo.absoluteFilePath();

        if (isExcluded(filepath)) {
            qDebug() << "excluded item: " << filepath;
            continue;
        }

        if (fileinfo.isSymLink()) {
            QString linkName = ReadLink(filepath);
            onLink(filepath, linkName);
        } else if (fileinfo.isDir()) {
            onEnterDir(filepath);
            recurseDirectory(filepath);
            onLeaveDir(filepath);
        } else if (fileinfo.isFile()) {
            onFile(filepath);
        } else {
            onOther(filepath);
        }
    }
}


/*! Is called during directory traversal if a file was found.
 *
 *  \param absoluteFilePath the absolute path to the found file
 */
void Traverser::onFile(const QString &absoluteFilePath)
{
    // do nothing
    qDebug() << "DirectoryTraverser: file:" << absoluteFilePath;
}


/*! Is called during directory traversal if a new directory is entered.
 *
 *  \param absoluteFilePath the absolute path to the new directory
 */
void Traverser::onEnterDir(const QString &absoluteFilePath)
{
    // do nothing
    qDebug() << "DirectoryTraverser: dir enter:" << absoluteFilePath;
}


/*! Is called during directory traversal if a directory is left.
 *
 *  \param absoluteFilePath the absolute path to the left directory
 */
void Traverser::onLeaveDir(const QString &absoluteFilePath)
{
    // do nothing
    qDebug() << "DirectoryTraverser: dir leave:" << absoluteFilePath;
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
void Traverser::onLink(const QString &absoluteFilePath, const QString &linkName)
{
    // do nothing
    qDebug() << "DirectoryTraverser: link:" << absoluteFilePath << linkName;
}


/*! Is called during directory traversal if a other item (no link, no file, no directory) was found.
 *
 *  \param absoluteFilePath the absolute path to the file system item
 */
void Traverser::onOther(const QString &absoluteFilePath)
{
    // do nothing
    qDebug() << "DirectoryTraverser: other:" << absoluteFilePath;
}
