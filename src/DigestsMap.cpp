/*
 * DigestsMap.cpp
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


#include "DigestsMap.h"
#include "Checksum.h"

#include <QFile>


/*! Constructs a new digest map object
 */
DigestsMap::DigestsMap()
{
    this->clear();
}


/*! Load the contents of an digests file
 *
 * \param filename the filename of the digests file to load
 *
 * \return true, if successful loaded contents from file, otherwise false
 */
QByteArray DigestsMap::load(const QString &filename)
{
    this->clear();

    QFile file(filename);
    int success = file.open(QIODevice::ReadOnly);
    if (!success) {
        return QByteArray();
    }

    Checksum checksum;

    for (;;) {
        QByteArray line = file.readLine();
        if (line.size() == 0) {
            break;
        }

        checksum.update(line);

        int firstblank = line.indexOf(' ');
        if (firstblank == -1) {
            break;
        }

        int endofline = line.indexOf('\n');
        if (endofline == -1) {
            break;
        }

        QByteArray digest = line.mid(0, firstblank);
        QByteArray path = line.mid(firstblank + 2, endofline - firstblank - 2);
        this->insert(QString::fromUtf8(path), digest);
    }

    return checksum.finish();
}


/*! Saved the contents of this digests map to a digest file.
 *
 * \param filename the filename of the digest file to save
 *
 * \return true, if successful saved contents to file, otherwise false
 */
QByteArray DigestsMap::save(const QString &filename)
{
    QFile target(filename);
    Checksum checksum;

    int success = target.open(QIODevice::WriteOnly);
    if (success) {
        DigestsMapIterator iter(*this);
        while (iter.hasNext()) {
            iter.next();
            QByteArray line;
            line.append(iter.value());
            line.append(" *");   // binary flag
            line.append(iter.key().toUtf8());
            line.append("\n");
            checksum.update(line);
            target.write(line);
        }
    }

    return success ? checksum.finish() : QByteArray();
}

