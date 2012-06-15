/*
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
#include <QRadialGradient>
#include <qmath.h>
#include <QDebug>

/*! Constructs a new capacity visualizing widget which is a child of \em parent.
 *
 * \param model the model containing drive space information
 * \param parent the parent UI control element
 */
DriveCapacityUI::DriveCapacityUI(AbstractDriveCapacityModel *model, QWidget *parent) : QWidget(parent)
{
    m_model = model;

    setBackgroundRole(QPalette::NoRole);
    setAutoFillBackground(true);

    connect(m_model, SIGNAL(dataChanged()), this, SLOT(update()));

    rotate(10,0);
    rotate(10,45);
    rotate(10,90);
    rotate(10,135);
    rotate(10,180);
    rotate(10,225);
    rotate(10,270);
    rotate(10,315);
    rotate(10,360);
}


QSize DriveCapacityUI::minimumSizeHint() const
{
    return QSize(100, 100);
}


QSize DriveCapacityUI::sizeHint() const
{
    return QSize(175, 175);
}


/*! Draws a shadow of the pie chart element
 *
 * \param painter the paint device
 * \param panel   the borders of the paint range
 * \param from    start angle of the pie chart segment
 * \param to      end angle of the pie chart segment
 */
void DriveCapacityUI::drawShadow(QPainter &painter, QRect &panel, int from, int to)
{
    painter.setPen(Qt::darkGray);
    painter.setBrush(Qt::darkGray);
    painter.drawPie(panel.adjusted(5, 5, 5, 5), from * 16, to * 16);
}


QPoint DriveCapacityUI::rotate(qreal scale, qreal angle)
{
    const qreal pi = 3.14159265358979;
    const qreal rotationStart = 315;
    QPoint point;

    qreal a = pi/180.0 * (angle + rotationStart);
    qreal sina = qSin(a);
    qreal cosa = qCos(a);

    scale /= 1.41421; // scale to unit circle
    qreal x = (cosa - sina) * scale;
    qreal y = (sina + cosa) * scale;

    point.setX(qRound(x));
    point.setY(-qRound(y));

    qDebug() << "rotate" << angle << x << y << point;

    return point;
}


/*! Draws the pie chart element
 *
 * \param painter the paint device
 * \param panel   the borders of the paint range
 * \param from    start angle of the pie chart segment
 * \param to      end angle of the pie chart segment
 * \param centerColor the color of the pie chart segment in the center
 * \param borderColor the color of the pie chart segment at the border
 */
void DriveCapacityUI::drawElement(QPainter &painter, QRect &panel, qreal from, qreal span, QColor centerColor, QColor borderColor)
{
    QPoint center = panel.center();

    QRadialGradient gradient(center.x(), center.y(), 75);
    gradient.setColorAt(0, centerColor);
    gradient.setColorAt(1, borderColor);

    painter.setBrush(gradient);
    painter.setPen(borderColor);
    painter.drawPie(panel, from * 360 * 16, span * 360 * 16);

    QString test;
    test.sprintf("%.1f%%", (float)(100.0 * span));

    painter.setBrush(borderColor);
    painter.setPen(Qt::black);

    QPoint rotated = rotate(40, (from + span/2) * 360);
    QPoint textCenter = center + rotated;

    QRect rect(textCenter.x()-40, textCenter.y()-20, 100, 40);
    painter.drawText(rect, Qt::AlignCenter, test);
}


void DriveCapacityUI::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    QRect panel = contentsRect().adjusted(10, 10, -10, -10);

    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setBackgroundMode(Qt::OpaqueMode);

    QColor freeColor(0, 255, 150);
    QColor lightFreeColor(200, 255, 200);
    QColor usedColor(255, 0, 0);
    QColor lightUsedColor(255, 200, 200);

    qreal capacity = m_model->capacity();

    capacity = 0.1575;
    //drawShadow(painter, panel, capacity * 360, 360);
    //drawShadow(painter, panel, 0, capacity * 360);

    drawElement(painter, panel, 0, capacity, lightFreeColor, freeColor);
    drawElement(painter, panel, capacity, 1.0-capacity, lightUsedColor, usedColor);
}
