/**
 * DriveUsageUI.cpp
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

#include "DriveUsageUI.h"
#include <QPainter>

DriveUsageUI::DriveUsageUI(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::NoRole);
    setAutoFillBackground(true);
}

QSize DriveUsageUI::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize DriveUsageUI::sizeHint() const
{
    return QSize(130, 130);
}

void DriveUsageUI::drawShadow(QPainter &painter, QRect &panel, int from, int to)
{
    painter.setPen(Qt::darkGray);
    painter.setBrush(Qt::darkGray);
    painter.drawPie(panel.adjusted(5,5,5,5), from * 16, to * 16);
}

void DriveUsageUI::drawElement(QPainter &painter, QRect &panel, int from, int to, QColor color)
{
    QPoint center = panel.center();

    QLinearGradient gradient(center.x(),
                             panel.top(),
                             center.x(),
                             3*panel.bottom()
                             );


    gradient.setColorAt(1,Qt::black);
    gradient.setColorAt(0,color);
    painter.setBrush(gradient);
    painter.setPen(color);

    painter.drawPie(panel, from * 16, to * 16);

    QString test;
    test = "hallo";
    painter.setBrush(color);
    painter.setPen(Qt::black);

    QPoint origin = center;
    QPoint rotated = origin + QPoint(20,20);

    painter.drawText(rotated, test);
}

void DriveUsageUI::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    QRect panel = contentsRect().adjusted(10,10,-10,-10);

    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setBackgroundMode(Qt::OpaqueMode);

    QColor freeColor(0,255,150);
    QColor usedColor(255,0,0);

    drawShadow(painter, panel, 90, 290);
    drawShadow(painter, panel, 0, 90);

    drawElement(painter, panel, 90, 290, freeColor);
    drawElement(painter, panel, 0, 90, usedColor);
}
