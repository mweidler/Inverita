/**
 * WorkerEngine.cpp
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


#include "WorkerEngine.h"


WorkerEngine::WorkerEngine()
{
    reset();
}

void WorkerEngine::reset()
{
    m_currentTask = 0;
    m_abort = false;
    m_descriptions.clear();
}

int WorkerEngine::taskCount()
{
    return m_descriptions.count();
}

QString WorkerEngine::taskDescription(int task)
{
    return m_descriptions[task];
}

int WorkerEngine::currentTask()
{
    return m_currentTask;
}

QString WorkerEngine::failureHint()
{
    return m_failureHint;
}

void WorkerEngine::buildFailureHint(ApplicationException &e)
{
    m_failureHint =
        tr("Causer: ") + e.causer() + "\n" +
        tr("Error: ") + e.errorMessage() + "\n";
}
