/*
 * ScanTraverser.cpp
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


#include "ScanTraverser.h"


/*! Default constructor
 */
ScanTraverser::ScanTraverser()
{
}


/*! Is called during directory traversal if a file was found.
 *
 *  \param absoluteFilePath the absolute path to the found file
 */
void ScanTraverser::onFile(const QString &absoluteFilePath)
{
    QFileInfo fileinfo(absoluteFilePath);

    m_totalFiles++;
    m_totalSize += fileinfo.size();
}


/*! Is called during directory traversal if a new directory is entered.
 *
 *  \param absoluteFilePath the absolute path to the new directory
 */
void ScanTraverser::onEnterDir(const QString &/*absoluteFilePath*/)
{
    // do nothing
}


/*! Is called during directory traversal if a directory is left.
 *
 *  \param absoluteFilePath the absolute path to the left directory
 */
void ScanTraverser::onLeaveDir(const QString &/*absoluteFilePath*/)
{
    // do nothing
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
void ScanTraverser::onLink(const QString &/*absoluteFilePath*/, const QString &/*linkName*/)
{
    // do nothing
}


/*! Is called during directory traversal if a other item (no link, no file, no directory) was found.
 *
 *  \param absoluteFilePath the absolute path to the file system item
 */
void ScanTraverser::onOther(const QString &/*absoluteFilePath*/)
{
    // do nothing
}
