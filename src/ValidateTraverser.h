/*
 * ValidateTraverser.h
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

#ifndef HEADER_VALIDATETRAVERSER_INC
#define HEADER_VALIDATETRAVERSER_INC

#include "Traverser.h"
#include "DigestsMap.h"
#include "SnapshotMetaInfo.h"


/*! Validates file contents.
 */
class ValidateTraverser : public Traverser
{
    Q_OBJECT

public:
    ValidateTraverser();

    DigestsMap &digests();
    void setBackupPath(const QString &absolutePath);
    void evaluate(SnapshotMetaInfo &metainfo);

    virtual void onFile(const QString &absoluteFilePath);

    TRAVERSER_IGNORE_ONENTERDIR
    TRAVERSER_IGNORE_ONLEAVEDIR
    TRAVERSER_IGNORE_ONLINK
    TRAVERSER_IGNORE_ONOTHER

private:
    QByteArray computeDigestOfFile(const QString &sourcefilename);

    QString    m_backupPath;       //!< absolute path to the current backup
    int        m_sizeOfBackupPath;
    QByteArray m_fileBuffer;       //!< buffer for file copying
    DigestsMap m_digests;          //!< digests of all files in current backup
};

#endif
