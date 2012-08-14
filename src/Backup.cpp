/*
 * Backup.cpp
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


#include "Backup.h"

#include <QDir>
#include <QProcess>

/*! Constructs a new backup.
 */
Backup::Backup()
{
    m_isOpen = false;
    m_encryption = Backup::NotEncrypted;
}


Backup::Backup(const QString &origin)
{
    m_isOpen = false;
    m_encryption = Backup::NotEncrypted;
    m_origin = origin;
}

/*! Destructor
 */
Backup::~Backup()
{

}

Backup &Backup::instance()
{
    static Backup backup;
    return backup;
}

void Backup::use(const QString &origin)
{
    m_isOpen = false;
    m_encryption = Backup::NotEncrypted;
    m_origin = origin;
}


Configuration &Backup::config()
{
    return m_config;
}

QString Backup::label() const
{
    return m_label;
}

void Backup::setLabel(const QString &label)
{
    m_label = label;
}

QString Backup::origin() const
{
    return m_origin;
}

void Backup::setOrigin(const QString &origin)
{
    m_origin = origin;
}

QString Backup::password() const
{
    return m_password;
}

void Backup::setPassword(const QString &password)
{
    m_password = password;
}

Backup::Encryption Backup::encryption() const
{
    return m_encryption;
}

void Backup::setEncryption(const Encryption encrypt)
{
    m_encryption = encrypt;
}

QString Backup::location() const
{
    return m_location;
}

bool Backup::isOpen() const
{
    return m_isOpen;
}

QString Backup::errorString() const
{
    return m_errorString;
}

int Backup::error() const
{
    return m_rc;
}


QString Backup::findUsableMountPoint() const
{
    QString basePath = QDir::homePath() + "/" + "inverita-backup";

    for (int i = 1; i < 100; i++) {
        QDir dir((i > 1) ? (basePath + QString("%1").arg(i)) : basePath);
        if (dir.exists() == false) {
            return dir.absolutePath();
        }
    }

    return QString("/tmp/inverita-backup");
}


Backup::Status Backup::open()
{
    QProcess process;
    QDir dir;

    switch (m_encryption) {
        case Backup::NotEncrypted:
            dir.setPath(m_origin);
            if (!dir.exists()) {
                m_errorString = tr("Directory does not exist or is not accessible.");
                return Backup::Failed;
            }
            m_location = m_origin;
            break;

        case Backup::EncFSEncrypted:
            m_location = findUsableMountPoint();
            dir.mkpath(m_location);

            // start encfs as daemon, reading password from stdin
            process.start("encfs", QStringList() << "--stdinpass" <<  "--standard" << m_origin << m_location);
            if (process.waitForStarted() == false) {
                m_errorString = process.errorString();
                m_rc = process.exitCode();
                dir.rmdir(m_location);
                return Backup::CouldNotStarted;
            }

            process.write(m_password.toUtf8());
            process.closeWriteChannel();
            process.waitForFinished();

            m_errorString = QString::fromLocal8Bit(process.readAllStandardOutput().append(process.readAllStandardError()));
            m_rc = process.exitCode();

            if (m_rc != 0) {
                dir.rmdir(m_location);
                return Backup::Failed;
            }
            break;

        default:
            return Backup::Failed;
            break;
    }

    m_isOpen = true;
    return Backup::Success;
}


Backup::Status Backup::close()
{
    QProcess process;
    QDir dir;

    if (!isOpen()) {
        return Backup::Success;
    }

    switch (m_encryption) {
        case Backup::NotEncrypted:
            break;

        case Backup::EncFSEncrypted:
            process.start("fusermount", QStringList() << "-u" << m_location);
            process.waitForStarted();
            process.waitForFinished();

            m_errorString = process.readAllStandardError();
            m_rc = process.exitCode();

            if (m_rc != 0) {
                return Backup::Failed;
            }

            dir.rmdir(m_location);
            break;

        default:
            return Backup::Failed;
            break;
    }

    m_isOpen = false;
    return Backup::Success;
}
