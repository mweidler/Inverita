/*
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

#include "ApplicationException.h"

#include <QtCore>
#include <QString>

#include <stdlib.h>
#include <string.h>
#include <iostream>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif


/*! Construct a new exception object
 */
ApplicationException::ApplicationException() throw()
{
}


/*! Destructor
 */
ApplicationException::~ApplicationException() throw()
{
}


/*! Describes, who causes the error/exception, e.g. "Write to disk"
 *
 * \param causer
 */
void ApplicationException::setCauser(QString causer)
{
    m_causer = causer;
}


/*! Describes, what kind of error happened, e.g. "No rights to write to disk"
 *
 * \param errormsg
 */
void ApplicationException::setErrorMessage(QString errormsg)
{
    m_errormsg = errormsg;
}


/*! Describes, what kind of error happened, e.g. "No rights to write to disk"
 *
 * \param errormsg
 */
void ApplicationException::setErrorMessage(char *errormsg)
{
    m_errormsg = QString(errormsg);
}


/*! \return the causer of the error/exception
 */
QString &ApplicationException::causer()
{
    return m_causer;
}


/*! \return the error message, describing the error/exception
 */
QString &ApplicationException::errorMessage()
{
    return m_errormsg;
}


/*! \return legacy std::string error/exception description
 */
const char *ApplicationException::what()
{
    return m_errormsg.toUtf8().data();
}

