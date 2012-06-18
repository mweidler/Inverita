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


#include <QtCore>
#include <QString>
#include <QStringList>

#include "Configuration.h"


Configuration::Configuration()
{
    reset();
}

void Configuration::reset()
{
    m_includePaths.clear();
    m_excludePatterns.clear();
    m_verify       = true;
    m_verifyHash   = true;
    m_verifyTime   = true;
    m_verifySize   = true;
    m_purgeBackups = true;
    m_limitBackups = true;
    m_maxBackups   = 50;
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
        fprintf(stderr, "Reading Config %d\n", i);
        settings.setArrayIndex(i);
        m_includePaths.append(settings.value("Include").toString());
    }
    settings.endArray();

    size = settings.beginReadArray("EXCLUDEPATTERNS");
    for (int i = 0; i < size; ++i) {
        fprintf(stderr, "Reading Config %d\n", i);
        settings.setArrayIndex(i);
        m_excludePatterns.append(settings.value("Exclude").toString());
    }
    settings.endArray();

    m_verify = settings.value("OPTIONS/Verify", true).toBool();
    m_verifyHash = settings.value("OPTIONS/VerifyHash", true).toBool();
    m_verifyTime = settings.value("OPTIONS/VerifyTime", true).toBool();
    m_verifySize = settings.value("OPTIONS/VerifySize", true).toBool();

    m_purgeBackups = settings.value("OPTIONS/PurgeBackups", true).toBool();
    m_limitBackups = settings.value("OPTIONS/LimitBackups", true).toBool();
    m_maxBackups   = settings.value("OPTIONS/MaxBackups", 50).toInt();

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

    settings.setValue("OPTIONS/Verify",      m_verify);
    settings.setValue("OPTIONS/VerifyHash",  m_verifyHash);
    settings.setValue("OPTIONS/VerifyTime",  m_verifyTime);
    settings.setValue("OPTIONS/VerifySize",  m_verifySize);

    settings.setValue("OPTIONS/PurgeBackups",  m_purgeBackups);
    settings.setValue("OPTIONS/LimitBackups",  m_limitBackups);
    settings.setValue("OPTIONS/MaxBackups",    m_maxBackups);

    if (settings.status() != QSettings::NoError) {
        ApplicationException e;
        e.setCauser("save settings '" + filename + "'");
        e.setErrorMessage(tr("Error during writing"));
        throw e;
    }
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

int Configuration::GetVerification()
{
    int verification = 0;

    if (m_verify) {
        verification |= VERIFY_ENABLED;
    }
    if (m_verifyHash) {
        verification |= VERIFY_CONTENT;
    }
    if (m_verifySize) {
        verification |= VERIFY_SIZE;
    }
    if (m_verifyTime) {
        verification |= VERIFY_TIME;
    }

    return verification;
}

void Configuration::SetVerification(int verification)
{
    m_verify     = (verification & VERIFY_ENABLED) ? true : false;
    m_verifyHash = (verification & VERIFY_CONTENT) ? true : false;
    m_verifySize = (verification & VERIFY_SIZE) ? true : false;
    m_verifyTime = (verification & VERIFY_TIME) ? true : false;
}

bool Configuration::GetBackupPurgeAllowed()
{
    return m_purgeBackups;
}

void Configuration::SetBackupPurgeAllowed(bool allowed)
{
    m_purgeBackups = allowed;
}

bool Configuration::GetBackupRestricted()
{
    return m_limitBackups;
}

void Configuration::SetBackupRestricted(bool restricted)
{
    m_limitBackups = restricted;
}

int Configuration::GetBackupRestriction()
{
    return m_maxBackups;
}

void Configuration::SetBackupRestriction(int restriction)
{
    m_maxBackups = restriction;
}
