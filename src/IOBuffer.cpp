/*
 * IOBuffer.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012 - 2013  Marc Weidler <marc.weidler@web.de>,
 *                            Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                            All rights reserved.
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
#include <QDebug>


/*! Constructs a new backup.
 */
IOBuffer::IOBuffer()
{
    m_elapsed = -1;
    m_buffer.resize(FileIoBufferSize);
}


/*! Destructor
 */
IOBuffer::~IOBuffer()
{

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
qint64 IOBuffer::chunkSize() const
{
    return FileIoBufferSize;
}


/*! Starts a new logical IO cycle
 */
void IOBuffer::begin()
{
    m_time.start();
}


/*! Finishes the logical IO cycle
 */
void IOBuffer::finished()
{
    m_elapsed = m_time.elapsed(); // in milliseconds
    //qDebug() << "Elapsed:" << m_elapsed;
}


/*! \return the duration of the logical IO cycle
 */
int IOBuffer::elapsed() const
{
    return m_elapsed;
}
