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

#include <QtCore>
#include <QString>
#include <QStringList>

#include "ApplicationException.h"

/*
#define VERIFY_ENABLED        1
#define VERIFY_CONTENT        2
#define VERIFY_SIZE           4
#define VERIFY_TIME           8
#define VERIFY_FLAGS         16
#define VERIFY_DELETION      32
#define VERIFY_ADDED         64
#define VERIFY_AFTER_BACKUP 128
*/
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
