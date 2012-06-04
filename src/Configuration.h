/*
 * ConfigurationUI.hpp
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

#ifndef HEADER_CONFIGURATION_INC
#define HEADER_CONFIGURATION_INC

#include <QtCore>
#include <QString>
#include <QStringList>

#include "ApplicationException.h"

#define VERIFY_ENABLED        1
#define VERIFY_CONTENT        2
#define VERIFY_SIZE           4
#define VERIFY_TIME           8
#define VERIFY_FLAGS         16
#define VERIFY_DELETION      32
#define VERIFY_ADDED         64
#define VERIFY_AFTER_BACKUP 128

class Configuration : public QObject
{
public:
    Configuration();

    void reset();
    void Load(QString filename);
    void Save(QString filename);

    QStringList   &GetIncludes();
    void           AddInclude(QString include);

    QStringList   &GetExcludes();
    void           AddExclude(QString exclude);

    int            GetVerification();
    void           SetVerification(int verification);

    bool           GetBackupPurgeAllowed();
    void           SetBackupPurgeAllowed(bool allowed);

    bool           GetBackupRestricted();
    void           SetBackupRestricted(bool restricted);

    int            GetBackupRestriction();
    void           SetBackupRestriction(int restriction);

protected:
    QStringList    m_includePaths;
    QStringList    m_excludePatterns;
    bool           m_verify;
    bool           m_verifyHash;
    bool           m_verifyTime;
    bool           m_verifySize;
    bool           m_purgeBackups;
    bool           m_limitBackups;
    int            m_maxBackups;
};

#endif
