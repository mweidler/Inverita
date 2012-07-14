/*
 * SignatureMap.cpp
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


#include "SignatureMap.h"

#include <QFile>


/*! Constructs a new signature map object
 */
SignatureMap::SignatureMap()
{
    this->clear();
}


/*! Load the contents of an signature file
 *
 * \param filename the filename of the signature file to load
 *
 * \return true, if successful loaded contents from file, otherwise false
 */
bool SignatureMap::load(const QString &filename)
{
    this->clear();

    QFile file(filename);
    int success = file.open(QIODevice::ReadOnly);
    if (!success) {
        return false;
    }

    for (;;) {
        QByteArray line = file.readLine();
        if (line.size() == 0) {
            break;
        }

        int firstblank = line.indexOf(' ');
        if (firstblank == -1) {
            break;
        }

        int endofline = line.indexOf('\n');
        if (endofline == -1) {
            break;
        }

        QByteArray hash = line.mid(0, firstblank);
        QByteArray path = line.mid(firstblank + 2, endofline - firstblank - 2);
        this->insert(QString::fromUtf8(path), hash);
    }

    return true;
}


/*! Saved the contents of this signature map to a signature file.
 *
 * \param filename the filename of the signature file to save
 *
 * \return true, if successful saved contents to file, otherwise false
 */
bool SignatureMap::save(const QString &filename) const
{
    QFile target(filename);

    int success = target.open(QIODevice::WriteOnly);
    if (success) {
        SignatureMapIterator iter(*this);
        while (iter.hasNext()) {
            iter.next();
            target.write(iter.value());
            target.write(" *");   // binary flag
            target.write(iter.key().toUtf8());
            target.write("\n");
        }
    }

    return success;
}

