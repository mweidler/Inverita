/*
 * SignatureMap.cpp
 *
 * Copyright (c) 2012, Marc Weidler <marc.weidler@web.de>
 * Ulrichstr. 12/1, 71672 Marbach, Germany
 * All rights reserved.
 *
 * 'ALVARA Personal Backup' is licensed under the BSD license (simplified, 2-clause).
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
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

