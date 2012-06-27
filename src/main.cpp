/*
 * main.cpp
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

#include "mainwindow.h"
#include <QApplication>


/*! The one and only main function and start point of the program
 *
 * \param argc number of command line arguments given
 * \param vector of pointers to the command line arguments
 *
 * \return status of the application
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

// TODO: cleanup this

    //"QGroupBox { border: 1px solid gray; border-radius: 3px; } "
    QString style("QGroupBox { border: 1px solid; border-radius: 3px; } "
                  //"QGroupBox::title { background-color: transparent; subcontrol-position: top left; padding: 10 3px; }"
                 );

    /*QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top center;
        padding: 0 3px;
        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                          stop: 0 #FFOECE, stop: 1 #FFFFFF);
    }*/
    // a.setStyleSheet(style);

// TODO: Replace QFrame by QWidget where no frame is needed/displayed

    MainWindow w;
    w.show();

    return a.exec();
}
