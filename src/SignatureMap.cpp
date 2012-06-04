/**
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


#include <QtCore>
#include <QFile>

#include "SignatureMap.h"
#include "Utilities.h"



/*! Default constructor
 */
SignatureMap::SignatureMap()
{
    this->clear();
}


void SignatureMap::Load(QString filename)
{
    this->clear();

    QFile file(filename);
    int success = file.open(QIODevice::ReadOnly);
    if (!success) {
        return;
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

        this->insert(path, hash);
    }
}


void SignatureMap::Save(QString filename)
{
    QFile target(filename);
    int success = target.open(QIODevice::WriteOnly);
    if (success) {
        SignatureMapIterator iter(*this);
        while (iter.hasNext()) {
            iter.next();
            target.write(iter.value());
            target.write(" *");   // binary flag
            target.write(iter.key().toAscii());
            target.write("\n");
        }
    }
}

