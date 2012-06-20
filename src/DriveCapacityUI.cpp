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

    //setBackgroundRole(QPalette::NoRole);
    //setAutoFillBackground(true);

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


/*! Returns a point rotated by \em angle degrees on a circle
 *  with a given \em radius and center \em (0,0).
 *
 *    0 degrees results in (1,0) * radius
 *   90 degrees results in (0,1) * radius
 *  180 degrees results in (-1,0) * radius
 *  270 degrees results in (0,-1) * radius
 *  360 degrees results in (1,0) * radius
 *  Every degree between is allowed.
 *
 * \param radius  the radius of the circle to rotate on
 * \param angle   the angle (0..360) to rotate by
 */
QPoint DriveCapacityUI::rotatedPoint(qreal radius, qreal angle)
{
    const qreal pi = 3.14159265358979;
    const qreal rotationStart = 315;

    qreal a = pi / 180.0 * (angle + rotationStart);
    qreal sina = qSin(a);
    qreal cosa = qCos(a);

    radius /= qSqrt(2.0); // divide by 1.41421 to reach unit circle scaling
    qreal x = (cosa - sina) * radius;
    qreal y = (sina + cosa) * radius;

    QPoint point;
    point.setX(qRound(x));
    point.setY(-qRound(y)); // (0,0) coords are in the left-upper corner: invert y

    return point;
}


/*! Draws the pie chart element
 *
 * \param painter the paint device
 * \param panel   the borders of the paint range
 * \param from    start angle of the pie chart segment
 * \param span    width of the pie chart segment
 * \param centerColor the color of the pie chart segment in the center
 * \param borderColor the color of the pie chart segment at the border
 */
void DriveCapacityUI::drawElement(QPainter &painter, qreal from, qreal span, QColor centerColor, QColor borderColor)
{
    if (span == 0) {
        return;
    }

    QPoint center = contentsRect().center();

    QRadialGradient gradient(center.x(), center.y(), 75);
    gradient.setColorAt(0, centerColor);
    gradient.setColorAt(1, borderColor);

    painter.setBrush(gradient);
    painter.setPen(painter.background().color());
    painter.drawPie(contentsRect(), from * 360 * 16, span * 360 * 16);

    if (span >= 0.05) {
        QString label = QString("%1%").arg(100.0 * span, 0, 'f', 1);

        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setBrush(borderColor);
        painter.setPen(Qt::black);

        QPoint labelCenter = center + rotatedPoint(40, (from + span / 2) * 360);

        QRect labelRect(labelCenter.x() - 40, labelCenter.y() - 20, 100, 40);
        painter.drawText(labelRect, Qt::AlignCenter, label);
    }
}


void DriveCapacityUI::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setBackgroundMode(Qt::OpaqueMode);

    QColor freeColor(0, 255, 150);
    QColor lightFreeColor(200, 255, 200);
    QColor usedColor(255, 0, 0);
    QColor lightUsedColor(255, 200, 200);

    qreal capacity = m_model->capacity();
    drawElement(painter, 0, capacity, lightFreeColor, freeColor);
    drawElement(painter, capacity, 1.0 - capacity, lightUsedColor, usedColor);
}
