/*
 * Configuration.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2014  Marc Weidler <marc.weidler@web.de>,
 *                          Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                          All rights reserved.
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

Configuration::~Configuration()
{

}

void Configuration::reset()
{
    m_includePaths.clear();
    m_excludePatterns.clear();
    m_verifyAfterBackup = true;
    m_verifyDigest = true;
    m_autoDeleteSnapshots = true;
    m_spareCapacity = 7;
    m_limitSnapshots = true;
    m_maximumSnapshots = 40;
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
        return false;
    }

    int size = settings.beginReadArray("INCLUDEPATHS");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_includePaths.append(settings.value("include").toString());
    }
    settings.endArray();

    size = settings.beginReadArray("EXCLUDEPATTERNS");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_excludePatterns.append(settings.value("exclude").toString());
    }
    settings.endArray();

    m_verifyAfterBackup   = settings.value("OPTIONS/verifyafterbackup", true).toBool();
    m_verifyDigest        = settings.value("OPTIONS/verifydigest", true).toBool();
    m_autoDeleteSnapshots = settings.value("OPTIONS/autodeletesnapshots", true).toBool();
    m_spareCapacity       = settings.value("OPTIONS/sparecapacity", 7).toInt();
    m_limitSnapshots      = settings.value("OPTIONS/limitsnapshots", true).toBool();
    m_maximumSnapshots    = settings.value("OPTIONS/maximumsnapshots", 40).toInt();

    return true;
}


bool Configuration::save(const QString &filename) const
{
    QFile::remove(filename);

    QSettings settings(filename, QSettings::IniFormat);
    if ((settings.status() != QSettings::NoError) ||
        settings.isWritable() == false) {
        return false;
    }

    settings.beginWriteArray("INCLUDEPATHS");
    for (int i = 0; i < m_includePaths.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("include", m_includePaths[i]);
    }
    settings.endArray();

    settings.beginWriteArray("EXCLUDEPATTERNS");
    for (int i = 0; i < m_excludePatterns.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("exclude", m_excludePatterns[i]);
    }
    settings.endArray();

    settings.setValue("OPTIONS/verifyafterbackup",   m_verifyAfterBackup);
    settings.setValue("OPTIONS/verifydigest",        m_verifyDigest);
    settings.setValue("OPTIONS/autodeletesnapshots", m_autoDeleteSnapshots);
    settings.setValue("OPTIONS/sparecapacity",       m_spareCapacity);
    settings.setValue("OPTIONS/limitsnapshots",      m_limitSnapshots);
    settings.setValue("OPTIONS/maximumsnapshots",    m_maximumSnapshots);

    if (settings.status() != QSettings::NoError) {
        return false;
    }

    return true;
}


bool Configuration::verifyAfterBackup() const
{
    return m_verifyAfterBackup;
}

void Configuration::setVerifyAfterBackup(bool enable)
{
    m_verifyAfterBackup = enable;
}


bool Configuration::verifyDigest() const
{
    return m_verifyDigest;
}

void Configuration::setVerifyDigest(bool enable)
{
    m_verifyDigest = enable;
}

bool Configuration::autoDeleteSnapshots() const
{
    return m_autoDeleteSnapshots;
}

void Configuration::setAutoDeleteSnapshots(bool enable)
{
    m_autoDeleteSnapshots = enable;
}

int Configuration::spareCapacity() const
{
    return m_spareCapacity;
}

void Configuration::setSpareCapacity(int spare)
{
    m_spareCapacity = spare;
}


bool Configuration::limitSnapshots() const
{
    return m_limitSnapshots;
}

void Configuration::setLimitSnapshots(bool enable)
{
    m_limitSnapshots = enable;
}

int  Configuration::maximumSnapshots() const
{
    return m_maximumSnapshots;
}

void Configuration::setMaximumSnapshots(int count)
{
    m_maximumSnapshots = count;
}

