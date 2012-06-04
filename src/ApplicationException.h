/**
 * ApplicationException.h
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

#ifndef HEADER_APPLICATIONEXCEPTION_INC
#define HEADER_APPLICATIONEXCEPTION_INC

#include <QString>
#include <exception>

class ApplicationException : public std::exception
{
public:
    ApplicationException() throw();

    // This declaration is not useless:
    // http://gcc.gnu.org/onlinedocs/gcc-3.0.2/gcc_6.html#SEC118
    virtual ~ApplicationException() throw();

    void setCauser(QString causer);
    void setErrorMessage(QString errormsg);
    void setErrorMessage(char *errormsg);
    virtual const char *what();

    QString &causer();
    int     errorCode();
    QString &errorMessage();

protected:
    QString m_causer;
    QString m_errormsg;
};

#endif
