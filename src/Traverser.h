/*
 * Traverser.h
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

#ifndef HEADER_TRAVERSER_INC
#define HEADER_TRAVERSER_INC

#include "ApplicationException.h"
#include "Configuration.h"

#include <QFileInfo>


#define TRAVERSER_IGNORE_ONFILE     void onFile(const QString &/*absoluteFilePath*/) { }
#define TRAVERSER_IGNORE_ONENTERDIR void onEnterDir(const QString &/*absoluteFilePath*/) { }
#define TRAVERSER_IGNORE_ONLEAVEDIR void onLeaveDir(const QString &/*absoluteFilePath*/) { }
#define TRAVERSER_IGNORE_ONLINK     void onLink(const QString &/*absoluteFilePath*/, const QString &/*linkName*/) { }
#define TRAVERSER_IGNORE_ONOTHER    void onOther(const QString &/*absoluteFilePath*/) { }


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

    void   reset();
    void   addIncludes(const QString &absolutePath);
    void   addIncludes(QStringList &absolutePaths);
    void   addExcludes(const char *absolutePath);
    void   addExcludes(const QString &absolutePath);
    void   addExcludes(QStringList &absolutePaths);

    qint64 files() const;
    qint64 processed() const;
    qint64 transferred() const;
    qint64 errors() const;
    bool   shouldAbort() const;

    void traverse();

signals:
    void report(QString message);

public slots:
    void abort();


protected:
    void countFile();
    void countProcessed(qint64 size);
    void countTransferred(qint64 size);
    void countError();

    virtual void onFile(const QString &absoluteFilePath) = 0;
    virtual void onEnterDir(const QString &absoluteFilePath) = 0;
    virtual void onLeaveDir(const QString &absoluteFilePath) = 0;
    virtual void onLink(const QString &absoluteFilePath, const QString &linkName) = 0;
    virtual void onOther(const QString &absoluteFilePath) = 0;


private:
    void recurseDirectory(const QString &dirname);
    bool isExcluded(const QString &filepath);

    qint64         m_files;           //!< number of traversed files
    qint64         m_processed;       //!< size of all traversed files
    qint64         m_transferred;     //!< size of all read/written files
    qint64         m_errors;          //!< number of errors
    bool           m_abort;           //!< flag to signal the traverser to abort as soon as possible
    QStringList    m_basePaths;       //!< list of directories (paths) to be traversed
    QStringList    m_excludePatterns; //!< list of patterns to indentify excludes
};

#endif

