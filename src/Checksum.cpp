/*
 * Checksum.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2013  Marc Weidler <marc.weidler@web.de>,
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


#include "Checksum.h"


/*! Constructs a new checksum object
 */
Checksum::Checksum()
{
    reset();
}


/*! Destructor
 */
Checksum::~Checksum()
{
    memset(&ctx, 0, sizeof(sha1_context));
}


/*! Reinitialize the checksum algorithm.
 *
 * This has to be called before calculating a new checksu.
 */
void Checksum::reset()
{
    sha1_starts(&ctx);
}


/*! Updates the checksum with the whole data in the given buffer
 *
 * \param buffer the buffer with data to update the checksum
 */
void Checksum::update(const QByteArray &buffer)
{
    sha1_update(&ctx, (const unsigned char *)buffer.data(), buffer.size());
}


/*! Updates the checksum with the first \em count bytes of the given data in the buffer
 *
 * \param buffer the buffer with data to update the checksum
 * \param count the number of elements to be taken into account
 */
void Checksum::update(const QByteArray &buffer, int count)
{
    count = qMin(count, buffer.size());
    sha1_update(&ctx, (const unsigned char *)buffer.data(), count);
}


/*! Filalizes the current checksum and returns it
 *
 * \return Filalized checksum
 */
QByteArray Checksum::finish()
{
    QByteArray digestoutput(20, '\0');
    sha1_finish(&ctx, (unsigned char *)digestoutput.data());
    return digestoutput.toHex();
}
