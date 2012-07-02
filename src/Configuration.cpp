/*
 * Configuration.cpp
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


#include "Configuration.h"

#include <QFile>
#include <QSettings>


Configuration::Configuration()
{
    reset();
}

void Configuration::reset()
{
    m_includePaths.clear();
    m_excludePatterns.clear();
    m_verifyAfterBackup = true;
    m_verifyHash   = true;
    m_verifyTime   = true;
    m_verifySize   = true;
    m_autoDeleteBackups = true;
    m_limitBackups = true;
    m_maxBackups   = 40;
}

QStringList &Configuration::GetIncludes()
{
    return m_includePaths;
}

void Configuration::AddInclude(QString include)
{
    m_includePaths.append(include);
}

QStringList &Configuration::GetExcludes()
{
    return m_excludePatterns;
}

void Configuration::AddExclude(QString exclude)
{
    m_excludePatterns.append(exclude);
}

bool Configuration::Load(QString filename)
{
    reset();

    if (QFile::exists(filename) == false) {
        return false;
    }

    QSettings settings(filename,  QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("load settings '" + filename + "'");
        e.setErrorMessage(tr("Can not be opened"));
        throw e;
    }

    int size = settings.beginReadArray("INCLUDEPATHS");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_includePaths.append(settings.value("Include").toString());
    }
    settings.endArray();

    size = settings.beginReadArray("EXCLUDEPATTERNS");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_excludePatterns.append(settings.value("Exclude").toString());
    }
    settings.endArray();

    m_encryptSnapshots  = settings.value("OPTIONS/EncryptSnapshots", false).toBool();
    m_verifyAfterBackup = settings.value("OPTIONS/VerifyAfterBackup", true).toBool();
    m_verifyHash        = settings.value("OPTIONS/VerifyHash", true).toBool();
    m_verifyTime        = settings.value("OPTIONS/VerifyTime", true).toBool();
    m_verifySize        = settings.value("OPTIONS/VerifySize", true).toBool();

    m_autoDeleteBackups = settings.value("OPTIONS/AutoDeleteBackups", true).toBool();
    m_limitBackups      = settings.value("OPTIONS/LimitBackups", true).toBool();
    m_maxBackups        = settings.value("OPTIONS/MaxBackups", 40).toInt();

    return true;
}


void Configuration::Save(QString filename)
{
    QSettings settings(filename,  QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save settings '" + filename + "'");
        e.setErrorMessage(tr("Can not be opened"));
        throw e;
    }

    settings.remove("");

    settings.beginWriteArray("INCLUDEPATHS");
    for (int i = 0; i < m_includePaths.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("Include", m_includePaths[i]);
    }
    settings.endArray();

    settings.beginWriteArray("EXCLUDEPATTERNS");
    for (int i = 0; i < m_excludePatterns.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("Exclude", m_excludePatterns[i]);
    }
    settings.endArray();

    settings.setValue("OPTIONS/EncryptSnapshots",  m_encryptSnapshots);
    settings.setValue("OPTIONS/VerifyAfterBackup", m_verifyAfterBackup);
    settings.setValue("OPTIONS/VerifyHash",        m_verifyHash);
    settings.setValue("OPTIONS/VerifyTime",        m_verifyTime);
    settings.setValue("OPTIONS/VerifySize",        m_verifySize);

    settings.setValue("OPTIONS/AutoDeleteBackups", m_autoDeleteBackups);
    settings.setValue("OPTIONS/LimitBackups",      m_limitBackups);
    settings.setValue("OPTIONS/MaxBackups",        m_maxBackups);

    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save settings '" + filename + "'");
        e.setErrorMessage(tr("Error during writing"));
        throw e;
    }
}

bool Configuration::verifyAfterBackup()
{
    return m_verifyAfterBackup;
}

void Configuration::setVerifyAfterBackup(bool enable)
{
    m_verifyAfterBackup = enable;
}


bool Configuration::verifyHash()
{
    return m_verifyHash;
}

void Configuration::setVerifyHash(bool enable)
{
    m_verifyHash = enable;
}

bool Configuration::verifyTime()
{
    return m_verifyTime;
}

void Configuration::setVerifyTime(bool enable)
{
    m_verifyTime = enable;
}


bool Configuration::verifySize()
{
    return m_verifySize;
}

void Configuration::setVerifySize(bool enable)
{
    m_verifySize = enable;
}


bool Configuration::autoDeleteBackups()
{
    return m_autoDeleteBackups;
}

void Configuration::setAutoDeleteBackups(bool enable)
{
    m_autoDeleteBackups = enable;
}


bool Configuration::limitBackups()
{
    return m_limitBackups;
}

void Configuration::setLimitBackups(bool enable)
{
    m_limitBackups = enable;
}

int  Configuration::maximumBackups()
{
    return m_maxBackups;
}

void Configuration::setMaximumBackups(int count)
{
    m_maxBackups = count;
}

bool  Configuration::encryptSnapshots()
{
    return m_encryptSnapshots;
}

void Configuration::setEncryptSnapshots(bool enable)
{
    m_encryptSnapshots = enable;
}


