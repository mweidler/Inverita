/*
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

    countFile();
    countProcessed(fileinfo.size());
}


/*! Is called during directory traversal if a other item (no link, no file, no directory) was found.
 *
 *  \param absoluteFilePath the absolute path to the file system item
 */
void ScanTraverser::onOther(const QString &absoluteFilePath)
{
    emit report(tr("WARNING: Ingoring '%1'").arg(absoluteFilePath) + "<br>");
}
