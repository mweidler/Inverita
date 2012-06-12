/**
 * PieChart.cpp
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

#include "PieChart.h"
#include <QPainter>

PieChart::PieChart(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::NoRole);
    setAutoFillBackground(true);
}

QSize PieChart::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize PieChart::sizeHint() const
{
    return QSize(150, 150);
}

void PieChart::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    //     painter.setPen(pen);
    //     painter.setBrush(brush);

    QColor freeColor(25, 255, 120);
    QColor usedColor(255, 50, 50);
    QColor backColor(20, 20, 200);
    QColor penColor(128, 128, 128);
    QBrush freeBrush(freeColor);
    QBrush usedBrush(usedColor);
    QBrush backBrush(backColor);

    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setPen(penColor);

    painter.setBrush(backBrush);
    //painter.drawRect(contentsRect());

    painter.setBrush(freeBrush);
    painter.drawPie(contentsRect(), 90 * 16, 290 * 16);

    painter.setBrush(usedBrush);
    painter.drawPie(contentsRect(), 0 * 16, 90 * 16);
}
