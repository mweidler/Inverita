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
static const qint64 FileIoBufferSize  = 1 * FileIoMegaByte;


/*! Buffer with io chunk optimization to perform maximum copy performance.
 *
 * IOBuffer is implemented as Singleton.
 *
 * Performance measurement on different systems and different media have shown
 * that small chunk sizes will result in poor IO performance with 'fuse-based' file
 * systems. Huge chunk size result in poor IO performance on native and 'fuse-based'
 * file systems. Thus, a medium chunk size of 1 MB is a good trade-off in both
 * environments.
 *
 * \code
 *  Performance measurement
 *
 *  Buffer size       |   8K  |  16K  |  32K  |  64K  |  128K |  256K |  1MB  |  4MB  |  16MB |  32MB |  64MB | 128MB |
 *  ------------------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
 *  Laptop native     | 29.49 | 28.85 | 28.05 | 27.76 | 27.87 | 27.25 |*27.19 | 27.76 | 33.41 | 35.51 | 35.09 | 35.66 |
 *  Laptop encfs      | 99.31 |103.12 | 98.01 |102.63 |100.02 |104.51 |*85.94 | 87.04 | 91.12 | 90.12 | 88.52 | 82.70 |
 *  Laptop usb native |       |       |       |       |       |       |       |       |       |       |       |       |
 *  Laptop usb encfs  |       |       |       |       |       |       |       |       |       |       |       |       |
 *  Laptop usb truec. |       |       |       |       |       |       |       |       |       |       |       |       |
 *  Laptop NFS        |       |       |       |       |       |       |       |       |       |       |       |       |
 *  PC     native     |124.64 |118.47 |118.13 |120.21 |119.32 |120.73 |*117.88|125.01 |130.99 |135.22 |140.63 |133.41 |
 *  PC     encfs      |112.52 |112.86 |109.06 |106.66 |*105.13|110.72 |121.81 |118.67 |111.51 |108.80 |105.64 |102.93 |
 *  PC     usb native |       |       |       |       |       |       |       |       |       |       |       |       |
 *  PC     usb encfs  |       |       |       |       |       |       |       |       |       |       |       |       |
 *  PC     usb truec. |       |       |       |183.41 |       |       |107.46 |       | 90.42 |       | 89.44 |       |
 *  ------------------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
 * \endcode
 */
class IOBuffer : public QObject
{
    Q_OBJECT

private:
    IOBuffer();
    virtual ~IOBuffer();

public:
    static IOBuffer &instance();

    QByteArray &buffer();
    qint64     chunkSize() const;

    void       begin();
    void       finished();
    int        elapsed() const;

private:
    QByteArray m_buffer;
    QTime      m_time;
    int        m_elapsed;
};

#endif
