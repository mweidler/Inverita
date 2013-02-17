/*
 * ControlUI.hpp
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

#ifndef HEADER_CONTROLUI_INC
#define HEADER_CONTROLUI_INC

#include <QWidget>
#include <QToolButton>


/*!
 * Control elements that contains buttons to start create or verify of a backup
 */
class ControlUI : public QWidget
{
    Q_OBJECT

public:
    enum LogicalButton { CreateButton, VerifyButton, AllButtons };

    ControlUI(QWidget *parent = 0);
    ~ControlUI();

    void setEnabledButtons(LogicalButton button, bool enabled);

signals:
    void startBackup();
    void startVerify();

private:
    QToolButton *m_btnCreate;  //!< Button to start creating a new backup snapshot
    QToolButton *m_btnVerify;  //!< Button to start verifying an existing backup
};

#endif
