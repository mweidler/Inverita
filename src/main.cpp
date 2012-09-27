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

#include "version.h"
#include "InveritaWindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

#ifndef QT_LARGEFILE_SUPPORT
#error Will not compile without 'large file support'
#endif


/*! The one and only main function and start point of the program
 *
 * \param argc number of command line arguments given
 * \param vector of pointers to the command line arguments
 *
 * \return status of the application
 */
int main(int argc, char *argv[])
{
    bool translationEnabled = true;
    QApplication app(argc, argv);

    for (int i = 0; i < argc; i++) {
        QString option(argv[i]);
        if (option.toLower().startsWith("-icontheme=")) {
            QString themeName = option.mid(option.lastIndexOf("=") + 1);
            QIcon::setThemeName(themeName);
        }

        if (option.toLower().startsWith("-menushaveicons=")) {
            QString haveIcons = option.mid(option.lastIndexOf("=") + 1);
            QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, haveIcons != "yes");
        }

        if (option.toLower().startsWith("-translation=")) {
            QString translation = option.mid(option.lastIndexOf("=") + 1);
            translationEnabled = (translation == "yes");
        }
    }

    QApplication::setApplicationName("inverita");
    QApplication::setApplicationVersion(QString(INVERITA_COMMIT_VERSION));
    QApplication::setWindowIcon(QIcon::fromTheme(QApplication::applicationName(),
                                QIcon(":/images/backup-icon.png")));

    QTranslator qtTranslator;
    qtTranslator.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QTranslator appTranslator;
    appTranslator.load(":translations/" + QApplication::applicationName() + "_" + QLocale::system().name());

    QLocale loc(QLocale::system().language());

    if (translationEnabled) {
        app.installTranslator(&qtTranslator);
        app.installTranslator(&appTranslator);
        QLocale::setDefault(loc);
    }

    InveritaWindow w;
    w.show();

    return app.exec();
}
