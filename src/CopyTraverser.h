/*
 * CopyTraverser.h
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

#ifndef HEADER_COPYTRAVERSER_INC
#define HEADER_COPYTRAVERSER_INC

#include "Traverser.h"
#include "DigestsMap.h"


/*! Copy file system items (file, link, dir, other) from one place to
 *  an other while using hard links for alreadz existing files on the other place.
 */
class CopyTraverser : public Traverser
{
    Q_OBJECT

public:
    CopyTraverser();
    DigestsMap &currentDigests();
    DigestsMap &previousDigests();
    void setCurrentBackupPath(QString &path);
    void setPreviousBackupPath(QString &path);

    void onFile(const QString &absoluteFilePath);
    void onEnterDir(const QString &absoluteFilePath);
    void onLeaveDir(const QString &absoluteFilePath);
    void onLink(const QString &absoluteFilePath, const QString &linkName);


protected:
    bool compareFiles(QString &newfile, QString &reference);
    bool copyFile(QString &sourcefilename, QString &targetfilename, QByteArray &hash);

    QString    m_currentBackupPath;  //!< absolute path to the current backup
    QString    m_previousBackupPath; //!< absolute path to the previous backup
    char       m_copyBuffer[4096];   //!< buffer for file copying
    DigestsMap m_currentDigests;     //!< hash digests of all files in current backup
    DigestsMap m_previousDigests;    //!< hash digests of all files in previous backup
};

#endif
