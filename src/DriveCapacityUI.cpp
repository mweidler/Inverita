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


/*! Constructs a new capacity visualizing object.
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
}


/*! \reimp
 */
QSize DriveCapacityUI::minimumSizeHint() const
{
    return QSize(100, 100);
}


/*! \reimp QWidget::sizeHint()
 */
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


/*! Draws the pie chart element
 *
 * \param painter the paint device
 * \param panel   the borders of the paint range
 * \param from    start angle of the pie chart segment
 * \param to      end angle of the pie chart segment
 * \param centerColor the color of the pie chart segment in the center
 * \param borderColor the color of the pie chart segment at the border
 */
void DriveCapacityUI::drawElement(QPainter &painter, QRect &panel, int from, int to, QColor centerColor, QColor borderColor)
{
    QPoint center = panel.center();

    QRadialGradient gradient(center.x(), center.y(), 75);
    gradient.setColorAt(0, centerColor);
    gradient.setColorAt(1, borderColor);

    painter.setBrush(gradient);
    painter.setPen(borderColor);
    painter.drawPie(panel, from * 16, to * 16);

    QString test;
    test.sprintf("%.1f%%", (float)(100.0 * m_model->capacity()));

    painter.setBrush(borderColor);
    painter.setPen(Qt::black);

    QPoint origin = center;
    QPoint rotated = origin + QPoint(20, 20);

    painter.drawText(rotated, test);
}


/*! \reimp
 */
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

    drawShadow(painter, panel, capacity * 360, 360);
    drawShadow(painter, panel, 0, capacity * 360);

    drawElement(painter, panel, capacity * 360, 360, lightFreeColor, freeColor);
    drawElement(painter, panel, 0, capacity * 360, lightUsedColor, usedColor);
}
