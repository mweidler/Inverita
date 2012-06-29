/*
 * EraseTraverser.cpp
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


#include "EraseTraverser.h"
#include <QFile>


/*! Default constructor
 */
EraseTraverser::EraseTraverser()
{
}


/*! Is called during directory traversal if a file was found.
 *
 *  \param absoluteFilePath the absolute path to the found file
 */
void EraseTraverser::onFile(const QString &absoluteFilePath)
{
    QFile file(absoluteFilePath);

    m_totalFiles++;
    m_totalSize += file.size();

    if (!file.remove()) {
        ApplicationException e;
        e.setCauser("remove file '" + file.fileName() + "'");
        e.setErrorMessage(file.errorString());
        throw e;
    }
}


/*! Is called during directory traversal if a new directory is entered.
 *
 *  \param absoluteFilePath the absolute path to the new directory
 */
void EraseTraverser::onEnterDir(const QString &/*absoluteFilePath*/)
{
    // do nothing
}


/*! Is called during directory traversal if a directory is left.
 *
 *  \param absoluteFilePath the absolute path to the left directory
 */
void EraseTraverser::onLeaveDir(const QString &absoluteFilePath)
{
    QDir dir;
    if (!dir.rmdir(absoluteFilePath)) {
        ApplicationException e;
        e.setCauser("remove directory '" + absoluteFilePath + "'");
        e.setErrorMessage(tr("Can not remove directory"));
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
void EraseTraverser::onLink(const QString &absoluteFilePath, const QString &/*linkName*/)
{
    QFile file(absoluteFilePath);
    if (!file.remove()) {
        ApplicationException e;
        e.setCauser("remove link '" + file.fileName() + "'");
        e.setErrorMessage(file.errorString());
        throw e;
    }
}


/*! Is called during directory traversal if a other item (no link, no file, no directory) was found.
 *
 *  \param absoluteFilePath the absolute path to the file system item
 */
void EraseTraverser::onOther(const QString &absoluteFilePath)
{
    ApplicationException e;
    e.setCauser("remove unknown file '" + absoluteFilePath + "'");
    e.setErrorMessage(tr("Don't know what to do"));
    throw e;
}
