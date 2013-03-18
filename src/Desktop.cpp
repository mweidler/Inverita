/*
 * Desktop.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2013  Marc Weidler <marc.weidler@web.de>,
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


#include "Desktop.h"

#include <QProcessEnvironment>
#include <QIcon>
#include <QDebug>


/*! Constructs a new Desktop object
 */
Desktop::Desktop()
{
}


/*! Destructor
 */
Desktop::~Desktop()
{
}


/* handle environmend dependencies.
   DESKTOP_SESSION identifies the desktop environment:

   Mate 1.2 & 1.4 (Linux Mint 13 and 14)
   -------------------------------------
   DESKTOP_SESSION contains 'default.desktop'
   inverita -determineicontheme="mateconftool-2 --get /desktop/mate/interface/icon_theme"
   ps -ef | grep mateconfd | wc -l   -> 2

   KDE (Kubuntu 12.04)
   -------------------

    QString environment = QProcessEnvironment::systemEnvironment().value("DESKTOP_SESSION");
    QString theme = QIcon::themeName();

    out << "Mate desktop: " << mateDetection.contains("mateconfd") << "\n";
    out << "KDE  desktop: " << mateDetection.contains("xsettings-kde") << "\n";

    QFile file("/tmp/inveritainfo");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "DESKTOP_SESSION is '" << environment << "'\n";

    out << "Theme-Name is '" << theme << "'\n";
     // optional, as QFile destructor will already do it:
    file.close();

*/


/*! Execute one command and return it's stdout output
 *
 * \param executable the command to be executed
 * \return the stdout output of the command
 */
QString Desktop::executeCommand(QString executable)
{
    QProcess process;
    QString command;
    QString parameters;

    int firstBlank = executable.indexOf(" ");
    if (firstBlank >= 0) {
        command = executable.left(firstBlank);
        parameters = executable.mid(firstBlank + 1);
    } else {
        command = executable;
        parameters = "";
    }

    process.start(command, parameters.split(" ", QString::SkipEmptyParts));
    if (process.waitForStarted() == false) {
        return "";
    }

    process.waitForFinished();

    QString answer = QString::fromLocal8Bit(process.readAllStandardOutput());
    if (process.exitStatus() != QProcess::NormalExit) {
        return "";
    }

    return answer;
}


/*! Tries to determine the currently running desktop type,
 *  like Mate, KDE or Gnome.
 *
 * \return the determined desktop, or Unknown
 */
Desktop::DesktopType Desktop::determineDesktopType()
{
    QString processlist = executeCommand("ps -ef");

    if (processlist.contains("mateconfd")) {
        qDebug("Mate detected");
        return Desktop::Mate;
    }

    if (processlist.contains("xsettings-kde")) {
        qDebug("KDE detected");
        return Desktop::KDE;
    }

    return Desktop::Unknown;
}


/*! Tries to determine the configured icon theme of the current
 *  dektop enviromnent. Qt 4.8 does not handle this correctly
 *  for modern DEs like Mate and Gnome3.
 *
 * \return the configured icon theme to be used
 */
QString Desktop::determineIconTheme()
{
    QString iconTheme;

    switch (determineDesktopType()) {
        case Desktop::Mate:
            iconTheme = executeCommand("mateconftool-2 --get /desktop/mate/interface/icon_theme");
            iconTheme = iconTheme.replace("\n", "");
            break;

        case Desktop::KDE:
        default:
            iconTheme = QIcon::themeName();
            break;
    }

    qDebug("Icon Theme: '%s'", iconTheme.toStdString().c_str());
    return iconTheme;
}

