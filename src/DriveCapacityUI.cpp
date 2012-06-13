/**
 * DriveCapacityUI.cpp
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

#include "DriveCapacityUI.h"
#include <QPainter>

DriveCapacityUI::DriveCapacityUI(AbstractDriveCapacityModel *model, QWidget *parent) : QWidget(parent)
{
    m_model = model;

    setBackgroundRole(QPalette::NoRole);
    setAutoFillBackground(true);

    connect(m_model, SIGNAL(dataChanged()), this, SLOT(update()));
}

QSize DriveCapacityUI::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize DriveCapacityUI::sizeHint() const
{
    return QSize(175, 175);
}

void DriveCapacityUI::drawShadow(QPainter &painter, QRect &panel, int from, int to)
{
    painter.setPen(Qt::darkGray);
    painter.setBrush(Qt::darkGray);
    painter.drawPie(panel.adjusted(5,5,5,5), from * 16, to * 16);
}

void DriveCapacityUI::drawElement(QPainter &painter, QRect &panel, int from, int to, QColor color)
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
    test.sprintf("%.1f%%", (float)(100.0 * m_model->capacity()));

    painter.setBrush(color);
    painter.setPen(Qt::black);

    QPoint origin = center;
    QPoint rotated = origin + QPoint(20,20);

    painter.drawText(rotated, test);
}

void DriveCapacityUI::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    QRect panel = contentsRect().adjusted(10,10,-10,-10);

    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setBackgroundMode(Qt::OpaqueMode);

    QColor freeColor(0,255,150);
    QColor usedColor(255,0,0);

    qreal capacity = m_model->capacity();

    drawShadow(painter, panel, capacity * 360, 360);
    drawShadow(painter, panel, 0, capacity * 360);

    drawElement(painter, panel, capacity * 360, 360, freeColor);
    drawElement(painter, panel, 0, capacity * 360, usedColor);
}
