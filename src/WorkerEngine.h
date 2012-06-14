/*
 * WorkerEngine.h
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

#ifndef HEADER_WORKERENGINE_INC
#define HEADER_WORKERENGINE_INC

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>

#include "ApplicationException.h"


/*! Container for meta information
 */
typedef struct WorkerStatus {
    QDateTime timestamp;
    qreal     completion;
    qint64    processed;
} WorkerStatus;


/*! Abstract worker engine base class for several worker job classes.
 *
 *  Worker classes are working in a different thread and can thus work
 *  independently and without blocking the user interface.
 */
class WorkerEngine : public QObject
{
    Q_OBJECT

public:
    WorkerEngine();

    void    reset();
    int     taskCount();
    QString taskDescription(int task);
    int     currentTask();
    QString failureHint();

    virtual WorkerStatus status() = 0;

public slots:
    virtual void start() = 0;
    virtual void abort() = 0;

signals:
    void started();
    void finished();
    void aborted();
    void failed();
    void report(QString message);

protected:
    void buildFailureHint(ApplicationException &e);

protected:
    int         m_currentTask;
    QStringList m_descriptions;
    bool        m_abort;
    QString     m_failureHint;
};

#endif
