/**
 * ApplicationException.cpp
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

#include <stdlib.h>
#include <string.h>
#include <iostream>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

#include "ApplicationException.h"

ApplicationException::ApplicationException() throw()
{
}

ApplicationException::~ApplicationException() throw()
{
}

void ApplicationException::setCauser(QString causer)
{
    m_causer = causer;
}

void ApplicationException::setErrorMessage(QString errormsg)
{
    m_errormsg = errormsg;
}

void ApplicationException::setErrorMessage(char *errormsg)
{
    m_errormsg = QString(errormsg);
}

QString &ApplicationException::causer()
{
    return m_causer;
}

QString &ApplicationException::errorMessage()
{
    return m_errormsg;
}

const char *ApplicationException::what()
{
    return m_errormsg.toStdString().c_str();
}
