/*
 * DigestsMap.h
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

#ifndef HEADER_DIGESTSMAP_INC
#define HEADER_DIGESTSMAP_INC

#include <QString>
#include <QHash>
#include <QHashIterator>
#include <QByteArray>

typedef QHash<QString, QByteArray> Digests;
typedef QHashIterator<QString, QByteArray> DigestsMapIterator;


/*! Digest container
 *
 * The digest is compatible to a FIPS-180-1 compliant SHA-1 implementation.
 * This means, that the digests can be validated by common standard tools,
 * e.g. sha1sum -c 'digestsfile'.
 */
class DigestsMap : public Digests
{
public:
    DigestsMap();
    ~DigestsMap();

    QByteArray load(const QString &filename);
    QByteArray save(const QString &filename);

};

#endif
