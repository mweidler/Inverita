/*
 * Checksum.cpp
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


#include "Checksum.h"


/*! Constructs a new checksum object
 */
Checksum::Checksum()
{
    reset();
}

Checksum::~Checksum()
{
    memset(&ctx, 0, sizeof(sha1_context));
}

void Checksum::reset()
{
    sha1_starts(&ctx);
}

void Checksum::update(QByteArray &buffer)
{
    sha1_update(&ctx, (const unsigned char *)buffer.constData(), buffer.size());
}

void Checksum::update(const char *buffer, int size)
{
    sha1_update(&ctx, (const unsigned char *)buffer, size);
}

QByteArray Checksum::finish()
{
    QByteArray digestoutput(20, '\0');
    sha1_finish(&ctx, (unsigned char *)digestoutput.data());
    return digestoutput.toHex();
}
