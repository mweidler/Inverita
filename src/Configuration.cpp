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

QStringList &Configuration::includes()
{
    return m_includePaths;
}

void Configuration::addInclude(const QString &include)
{
    m_includePaths.append(include);
}

QStringList &Configuration::excludes()
{
    return m_excludePatterns;
}

void Configuration::addExclude(const QString &exclude)
{
    m_excludePatterns.append(exclude);
}

bool Configuration::load(const QString &filename)
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

    m_verifyAfterBackup = settings.value("OPTIONS/VerifyAfterBackup", true).toBool();
    m_verifyHash        = settings.value("OPTIONS/VerifyHash", true).toBool();
    m_verifyTime        = settings.value("OPTIONS/VerifyTime", true).toBool();
    m_verifySize        = settings.value("OPTIONS/VerifySize", true).toBool();

    m_autoDeleteBackups = settings.value("OPTIONS/AutoDeleteBackups", true).toBool();
    m_spareCapacity     = settings.value("OPTIONS/SpareCapacity", 7).toInt();
    m_limitBackups      = settings.value("OPTIONS/LimitBackups", true).toBool();
    m_maxBackups        = settings.value("OPTIONS/MaxBackups", 40).toInt();

    return true;
}


void Configuration::save(const QString &filename) const
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

    settings.setValue("OPTIONS/VerifyAfterBackup", m_verifyAfterBackup);
    settings.setValue("OPTIONS/VerifyHash",        m_verifyHash);
    settings.setValue("OPTIONS/VerifyTime",        m_verifyTime);
    settings.setValue("OPTIONS/VerifySize",        m_verifySize);

    settings.setValue("OPTIONS/AutoDeleteBackups", m_autoDeleteBackups);
    settings.setValue("OPTIONS/SpareCapacity",     m_spareCapacity);
    settings.setValue("OPTIONS/LimitBackups",      m_limitBackups);
    settings.setValue("OPTIONS/MaxBackups",        m_maxBackups);

    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save settings '" + filename + "'");
        e.setErrorMessage(tr("Error during writing"));
        throw e;
    }
}

bool Configuration::verifyAfterBackup() const
{
    return m_verifyAfterBackup;
}

void Configuration::setVerifyAfterBackup(bool enable)
{
    m_verifyAfterBackup = enable;
}


bool Configuration::verifyHash() const
{
    return m_verifyHash;
}

void Configuration::setVerifyHash(bool enable)
{
    m_verifyHash = enable;
}

bool Configuration::verifyTime() const
{
    return m_verifyTime;
}

void Configuration::setVerifyTime(bool enable)
{
    m_verifyTime = enable;
}


bool Configuration::verifySize() const
{
    return m_verifySize;
}

void Configuration::setVerifySize(bool enable)
{
    m_verifySize = enable;
}


bool Configuration::autoDeleteBackups() const
{
    return m_autoDeleteBackups;
}

void Configuration::setAutoDeleteBackups(bool enable)
{
    m_autoDeleteBackups = enable;
}

int Configuration::spareCapacity() const
{
    return m_spareCapacity;
}

void Configuration::setSpareCapacity(int spare)
{
    m_spareCapacity = spare;
}


bool Configuration::limitBackups() const
{
    return m_limitBackups;
}

void Configuration::setLimitBackups(bool enable)
{
    m_limitBackups = enable;
}

int  Configuration::maximumBackups() const
{
    return m_maxBackups;
}

void Configuration::setMaximumBackups(int count)
{
    m_maxBackups = count;
}

