/*
 * IOBuffer.h
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

#ifndef HEADER_IOBUFFER_INC
#define HEADER_IOBUFFER_INC

#include <QByteArray>
#include <QTime>


static const qint64 FileIoKiloByte    = 1024;
static const qint64 FileIoMegaByte    = 1024 * FileIoKiloByte;
static const qint64 FileIoBufferSize  = 64 * FileIoMegaByte;  //  64 MB
static const qint64 FileIoMinimumSize = 32 * FileIoKiloByte;  //  32 kB
static const qint64 FileIoMaximumSize = FileIoBufferSize;


/*! Buffer with io chunk optimization to perform maximum copy performance.
 *
 * IOBuffer is implemented as Singleton.
 */
class IOBuffer : public QObject
{
    Q_OBJECT

private:
    IOBuffer();
    virtual ~IOBuffer();

public:
    static IOBuffer &instance();

    void       reset();
    qint64     chunkSize();
    int        elapsed();
    QByteArray &buffer();
    void       begin();
    void       finished();

protected:
    void       determineOptimumChunkSize();

private:
    QByteArray m_buffer;
    QTime      m_time;
    qint64     m_chunksize;
    int        m_elapsed;
};

#endif
