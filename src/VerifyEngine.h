/*
 * VerifyEngine.h
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

#ifndef HEADER_VERIFYENGINE_INC
#define HEADER_VERIFYENGINE_INC

#include "WorkerEngine.h"
#include "ValidateTraverser.h"
#include "SnapshotMetaInfo.h"


/*! Implementation of a WorkerEngine that handels snapshot verification.
 */
class VerifyEngine : public WorkerEngine
{
    Q_OBJECT

public:
    VerifyEngine();

    int          taskCount() const;
    int          currentTask() const;
    QVariant     taskData(int task, int role) const;
    WorkerStatus status() const;

public slots:
    void start();
    void abort();

protected:
    int               m_currentTask;
    ValidateTraverser m_validateTraverser;
    SnapshotMetaInfo  m_metaInfo;
};

#endif
