/*
 * Configuration.hpp
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

#ifndef HEADER_CONFIGURATION_INC
#define HEADER_CONFIGURATION_INC

#include "ApplicationException.h"

//#i//nclude <QtCore>
#include <QString>
#include <QStringList>



class Configuration : public QObject
{
public:
    Configuration();

    void reset();

    QStringList &GetIncludes();
    void        AddInclude(QString include);

    QStringList &GetExcludes();
    void        AddExclude(QString exclude);

    bool verifyAfterBackup();
    void setVerifyAfterBackup(bool enable);

    bool verifyHash();
    void setVerifyHash(bool enable);

    bool verifyTime();
    void setVerifyTime(bool enable);

    bool verifySize();
    void setVerifySize(bool enable);

    bool autoDeleteBackups();
    void setAutoDeleteBackups(bool enable);

    bool limitBackups();
    void setLimitBackups(bool enable);

    int  maximumBackups();
    void setMaximumBackups(int count);

    bool Load(QString filename);
    void Save(QString filename);

protected:
    QStringList    m_includePaths;
    QStringList    m_excludePatterns;
    bool           m_verifyAfterBackup;
    bool           m_verifyHash;
    bool           m_verifyTime;
    bool           m_verifySize;
    bool           m_autoDeleteBackups;
    bool           m_limitBackups;
    int            m_maxBackups;
};

#endif
