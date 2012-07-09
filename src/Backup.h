/*
 * Backup.h
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

#ifndef HEADER_BACKUP_INC
#define HEADER_BACKUP_INC

#include <QString>
#include <QByteArray>
#include <QObject>


/*! Container class for storing backup information.
 */
class Backup// : public QObject
{
public:
    //Q_OBJECT

    enum Encryption { NotEncrypted = 0, EncFSEncrypted, TruecryptEncrypted };
    enum Status { Failed = 0, CouldNotStarted, Success };

    Backup();
    Backup(const QString &origin);
    virtual ~Backup();

    QString origin() const;
    void setOrigin(const QString &origin);

    QString password() const;
    void setPassword(const QString &password);

    Encryption encryption() const;
    void setEncryption(const Encryption encrypt);

    QString location() const;

    QString errorString() const;
    int error() const;

    Status open();
    Status close();
    bool isOpen() const;

    Encryption detectEncryption() const;
    static Encryption detectEncryption(const QString &origin);

private:
    QString findUsableMountPoint() const;

private:
    QString    m_origin;
    QString    m_password;
    Encryption m_encryption;
    QString    m_location;
    bool       m_isOpen;
    QString    m_errorString;
    int        m_rc;
};

#endif
