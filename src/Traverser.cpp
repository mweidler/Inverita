/*
 * Traverser.cpp
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


#include "Traverser.h"
#include "Utilities.h"

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
    m_files = 0;
    m_processed = 0;
    m_transferred = 0;
    m_errors = 0;
    m_abort = false;
    m_basePaths.clear();
    m_excludePatterns.clear();
}


/*! Add a specific directory (path) to the list to be traversed.
 */
void Traverser::addIncludes(const QString &absolutePath)
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
void Traverser::addExcludes(const QString &absolutePath)
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
qint64 Traverser::files() const
{
    return m_files;
}


/*! \returns the size of all traversed files.
 */
qint64 Traverser::processed() const
{
    return m_processed;
}

/*! \returns the size of all transferred (read/write) files.
 */
qint64 Traverser::transferred() const
{
    return m_transferred;
}


/*! \returns the number of occured errors.
 */
qint64 Traverser::errors() const
{
    return m_errors;
}


void Traverser::countFile()
{
    m_files++;
}


void Traverser::countProcessed(qint64 size)
{
    m_processed += size;
}


void Traverser::countTransferred(qint64 size)
{
    m_transferred += size;
}


void Traverser::countError()
{
    m_errors++;
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
    m_abort = true;
}


bool Traverser::shouldAbort() const
{
    return m_abort;
}


/*! Start traversal over all given paths recusively and call
 *  the appropriate handler methods.
 */
void Traverser::traverse()
{
    for (int i = 0; i < m_basePaths.count() && !m_abort; i++) {
        QString currentBasePath = m_basePaths[i];

        if (isExcluded(currentBasePath)) {
            continue;
        }

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
        e.setCauser(tr("Access to directory '%1' failed.").arg(dirname));
        e.setErrorMessage(tr("Directory not found."));
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
            continue;
        }

        if (fileinfo.isSymLink()) {
            QString linkName = ReadSymbolicLink(filepath);
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
