/*
 * WorkerEngine.h
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

#ifndef HEADER_WORKERENGINE_INC
#define HEADER_WORKERENGINE_INC

#include "ApplicationException.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>


/*! Container for meta information
 */
typedef struct WorkerStatus {
    QDateTime timestamp;
    qreal     completion;
    qint64    transferred;
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
    ~WorkerEngine();

    void     reset();
    QString  failureHint();

    virtual int      taskCount() const = 0;
    virtual int      currentTask() const = 0;
    virtual QVariant taskData(int task, int role) const = 0;
    virtual WorkerStatus status() const = 0;

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
    QString m_failureHint;
    bool    m_abort;
};

#endif
