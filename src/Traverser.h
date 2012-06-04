/*
 * Traverser.h
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
