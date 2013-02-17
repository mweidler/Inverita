/*
 * EraseEngine.h
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012 - 2013  Marc Weidler <marc.weidler@web.de>,
 *                            Ulrichstr. 12/1, 71672 Marbach, Germany.
 *                            All rights reserved.
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

#ifndef HEADER_ERASEENGINE_INC
#define HEADER_ERASEENGINE_INC

#include <QString>

#include "WorkerEngine.h"
#include "EraseTraverser.h"
#include "SnapshotMetaInfo.h"


/*! Handles the whole job of erasing a backup snapshot.
 */
class EraseEngine : public WorkerEngine
{
    Q_OBJECT

public:
    EraseEngine();

    int          taskCount() const;
    int          currentTask() const;
    QVariant     taskData(int task, int role) const;
    WorkerStatus status() const;

    void select(const QString &snapshotName);

public slots:
    void start();
    void abort();

protected:
    int              m_currentTask;
    QString          m_snapshotName;
    EraseTraverser   m_eraseTraverser;
    SnapshotMetaInfo m_metaInfo;
};

#endif
