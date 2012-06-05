/**
 * Traverser.h
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

#ifndef HEADER_TRAVERSER_INC
#define HEADER_TRAVERSER_INC

#include <QString>
#include <QStringList>

#include "ApplicationException.h"
#include "Configuration.h"


/*! Traverse recursively through all files and subdirectories and calls
 *  appropriate methods for each found item type (file, dir, link and other).
 *
 *  This class should be used as a base for special job classes, that
 *  do something with file system items, e.g. copying, deleting, etc.
 */
class Traverser : public QObject
{
    Q_OBJECT

public:
    Traverser();

public:
    void   reset();
    void   addIncludes(QString &absolutePath);
    void   addIncludes(QStringList &absolutePaths);
    void   addExcludes(const char *absolutePath);
    void   addExcludes(QString &absolutePath);
    void   addExcludes(QStringList &absolutePaths);
    qint64 totalFiles();
    qint64 totalSize();

    virtual void onFile(const QString &absoluteFilePath);
    virtual void onEnterDir(const QString &absoluteFilePath);
    virtual void onLeaveDir(const QString &absoluteFilePath);
    virtual void onLink(const QString &absoluteFilePath, const QString &linkName);
    virtual void onOther(const QString &absoluteFilePath);

public slots:
    void traverse();
    void abort();

protected:
    void RecurseDirectory(const QString &dirname);
    bool isExcluded(const QString &filepath);

protected:
    QStringList    m_basePaths;       //!< list of directories (paths) to be traversed
    QStringList    m_excludePatterns; //!< list of patterns to indentify excludes
    qint64         m_totalFiles;      //!< number of traversed files
    qint64         m_totalSize;       //!< size of all traversed files
    bool           m_abort;           //!< flag to signal the traverser to abort as soon as possible
    QReadWriteLock m_lock;            //!< lock semaphore for thread-safe read/write to statistics
};

#endif
