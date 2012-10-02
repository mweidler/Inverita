/*
 * IOBuffer.cpp
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


#include "IOBuffer.h"
#include "Utilities.h"
#include <QDebug>


/*! Constructs a new backup.
 */
IOBuffer::IOBuffer()
{
    reset();
    m_buffer.resize(FileIoBufferSize);
}


/*! Destructor
 */
IOBuffer::~IOBuffer()
{

}


/*! Reset chunk size to minimum size
 */
void IOBuffer::reset()
{
    m_chunksize = FileIoMinimumSize;
    m_elapsed = -1;
}


/*! \return Returns an instance of the current backup (singleton)
 */
IOBuffer &IOBuffer::instance()
{
    static IOBuffer iobuffer;
    return iobuffer;
}


/*! \return the buffer for read/write operation
 */
QByteArray &IOBuffer::buffer()
{
    return m_buffer;
}


/*! \return the optimal chunk size for reading/writing
 */
qint64 IOBuffer::chunkSize()
{
    return m_chunksize;
}


/*! Starts a new logical IO cycle
 */
void IOBuffer::begin()
{
    determineOptimumChunkSize();
    m_time.start();
}


/*! Finishes the logical IO cycle
 */
void IOBuffer::finished()
{
    m_elapsed = m_time.elapsed(); // in milliseconds
    qDebug() << "Chunksize:" << formatSize(m_chunksize) << "Elapsed:" << m_elapsed;
}


/*! \return the duration of the logical IO cycle
 */
int IOBuffer::elapsed()
{
    return m_elapsed;
}


/*! Determine the optimal buffer size for read/write cycle.
 *  One read/write (copy) cycle should take around 500 ms to
 *  give a good progress indicator of the user interface.
 */
void IOBuffer::determineOptimumChunkSize()
{
    if (m_elapsed == 0) {
        m_chunksize *= 2;
    } else if (m_elapsed > 0) {
        qreal bps = m_chunksize / (m_elapsed / 1000.0);

        if (m_elapsed < 300 || m_elapsed > 700) {
            m_chunksize = qRound(bps * 0.5); // IO should take half a second
        }
    }

    if (m_chunksize > FileIoMegaByte) {
        m_chunksize /= FileIoMegaByte;
        m_chunksize *= FileIoMegaByte; // floor to MB chunk
    } else {
        m_chunksize /= FileIoKiloByte;
        m_chunksize *= FileIoKiloByte; // floor to kB chunk
    }

    m_chunksize = FileIoMinimumSize;
    m_chunksize = qBound(FileIoMinimumSize, m_chunksize, FileIoMaximumSize);
}
