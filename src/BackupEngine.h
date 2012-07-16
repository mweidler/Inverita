/*
 * BackupEngine.h
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

#ifndef HEADER_BACKUPENGINE_INC
#define HEADER_BACKUPENGINE_INC

#include <QString>
#include <QStringList>

#include "Configuration.h"
#include "EraseTraverser.h"
#include "ScanTraverser.h"
#include "CopyTraverser.h"
#include "ValidateTraverser.h"
#include "WorkerEngine.h"


/*! Handles the whole job of creating a new backup snapshot.
 */
class BackupEngine : public WorkerEngine
{
    Q_OBJECT

public:
    BackupEngine();

    WorkerStatus status();

public slots:
    void start();
    void abort();

protected:
    void checkDriveSpace();
    void scanDirectories();
    void executeBackup(QString &timestamp);
    void validateBackup(QString &timestamp);
    void checkOvercharge();

    void deleteSnapshot(QString snapshotName);

protected:
    Configuration     m_config;
    QString           m_latestBackup;
    QString           m_currentBackup;
    EraseTraverser    m_eraseTraverser;
    ScanTraverser     m_scanTraverser;
    CopyTraverser     m_copyTraverser;
    ValidateTraverser m_validateTraverser;
};

#endif
