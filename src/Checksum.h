/*
 * Checksum.h
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

#ifndef HEADER_CHECKSUM_INC
#define HEADER_CHECKSUM_INC

#include "sha1.h"
#include <QByteArray>


/*! The checksum is compatible to a FIPS-180-1 compliant SHA-1 implementation.
 *  This means, that the digests can be validated by common standard tools,
 *  e.g. sha1sum -c 'digestsfile'.
 *
 * \code
 * Usage: Checksum checksum;
 *        checksum.reset();
 *        for x times {
 *            checksum.update(...);
 *            }
 *        QByteArray code = checksum.finish();
 * \endcode
 *
 */
class Checksum
{
public:
    Checksum();
    ~Checksum();

    void reset();
    void update(const QByteArray &buffer);
    void update(const QByteArray &buffer, int count);
    QByteArray finish();

private:
    sha1_context ctx;
};

#endif
