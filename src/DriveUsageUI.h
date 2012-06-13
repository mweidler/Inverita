/**
 * DriveUsageUI.h
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


#ifndef HEADER_DRIVEUSAGEUI_INC
#define HEADER_DRIVEUSAGEUI_INC

#include <QWidget>
#include <QColor>

#include "FilesystemInfo.h"

class DriveUsageUI : public QWidget
{
    Q_OBJECT

public:
    DriveUsageUI(FilesystemInfo *filesystemInfo, QWidget *parent);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *event);

private:
    void drawShadow(QPainter &painter, QRect &panel, int from, int to);
    void drawElement(QPainter &painter, QRect &panel, int from, int to, QColor color);

    FilesystemInfo *m_filesystemInfo;
};

#endif
