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
    m_encryption = Backup::NotEncrypted;
    m_isOpen = false;
    m_rc = 0;
}


/*! Constructs a new backup with an origin.
 *
 * \param origin filepath meaning the storage location of the backup
 */
Backup::Backup(const QString &origin)
{
    m_origin = origin;
    m_encryption = Backup::NotEncrypted;
    m_isOpen = false;
    m_rc = 0;
}


/*! Destructor
 */
Backup::~Backup()
{

}


/*! \return Returns an instance of the current backup (singleton)
 */
Backup &Backup::instance()
{
    static Backup backup;
    return backup;
}


/*! \return filepath meaning the storage location of the backup
 */
const QString &Backup::origin() const
{
    return m_origin;
}


/*! \return filepath where files could be read or written (filesystem for backup)
 */
const QString &Backup::location() const
{
    return m_location;
}


/*! \return true, if backup is open otherwise false
 */
bool Backup::isOpen() const
{
    return m_isOpen;
}


/*! Change/Set the location of the backup.
 *
 * \param origin filepath meaning the storage location of the backup
 */
void Backup::use(const QString &origin)
{
    m_origin = origin;
    m_encryption = Backup::NotEncrypted;
    m_isOpen = false;
}


/*! \return the configuration object of the current backup
 */
Configuration &Backup::config()
{
    return m_config;
}


/*! \return the label of the current backup
 */
const QString &Backup::label() const
{
    return m_label;
}


/*! Set the label of the current backup
 *
 * \param label the new label of the backup
 */
void Backup::setLabel(const QString &label)
{
    m_label = label;
}


/*! \return the password of the current backup or empty string, if not available
 */
const QString &Backup::password() const
{
    return m_password;
}


/*! Set the password of the current backup
 *
 * \param password the new password for this backup (if encrypted)
 */
void Backup::setPassword(const QString &password)
{
    m_password = password;
}


/*! \return the encryption type of the backup
 */
Backup::Encryption Backup::encryption() const
{
    return m_encryption;
}


/*! Set the encryption password of the current backup
 */
void Backup::setEncryption(const Encryption encrypt)
{
    m_encryption = encrypt;
}


/*! \return the error description of the last error occured
 */
const QString &Backup::errorString() const
{
    return m_errorString;
}


/*! \return the error code of the last error occured
 */
int Backup::error() const
{
    return m_rc;
}


/*! \return a mountpoint, that can be used for mounting an encrypted file system
 */
QString Backup::findUsableMountPoint() const
{
    QString basePath = QDir::homePath() + "/inverita-backup";

    for (int i = 1; i < 100; i++) {
        QDir dir((i > 1) ? (basePath + QString("%1").arg(i)) : basePath);
        if (dir.exists() == false) {
            return dir.absolutePath();
        }
    }

    return QString("/tmp/inverita-backup");
}


/*! Open a new backup (encrypted or unencrypted) and sets the location
 *
 * \return success, failed or what else
 */
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
            if (process.exitStatus() != QProcess::NormalExit) {
                m_rc = 1;
            }

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


/*! Close the current backup (encrypted or unencrypted)
 *
 * \return success, failed or what else
 */
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

