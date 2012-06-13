/**
 * FilesystemInfo.h
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


#ifndef HEADER_FILESYSTEMINFO_INC
#define HEADER_FILESYSTEMINFO_INC

#include <QObject>

#ifndef __USE_FILE_OFFSET64
   will not compile
#endif

#ifndef __USE_LARGEFILE64
   will not compile
#endif

#include <sys/vfs.h>

class FilesystemInfo : public QObject
{
    Q_OBJECT

public:
    FilesystemInfo();
    FilesystemInfo(const QString &file);
    void setFile(const QString &file);
    qreal usedCapacity();

signals:
    void changed();

private:
    void readStat();

    QString m_absolutePath;
    struct statfs64 m_st;
};

#endif
