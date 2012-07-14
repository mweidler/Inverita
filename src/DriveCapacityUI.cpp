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
#include "Utilities.h"

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

    m_freeColor = QColor(0, 235, 100);
    m_usedColor = QColor(235, 0, 0);

    connect(m_model, SIGNAL(dataChanged()), this, SLOT(update()));
}


/*! \copydoc QWidget::minimumSizeHint()
 */
QSize DriveCapacityUI::minimumSizeHint() const
{
    return QSize(270, 100);
}


/*! \copydoc QWidget::sizeHint()
 */
QSize DriveCapacityUI::sizeHint() const
{
    return minimumSizeHint();
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
QPoint DriveCapacityUI::rotatedPoint(qreal radius, qreal angle) const
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

    QPoint center = m_pieChartRect.center();

    QRadialGradient gradient(center.x(), center.y(), 75);
    gradient.setColorAt(0, centerColor);
    gradient.setColorAt(1, borderColor);

    painter.setBrush(gradient);
    painter.setPen(painter.background().color());
    painter.drawPie(m_pieChartRect, from * 360 * 16, span * 360 * 16);

    if (span >= 0.1) {
        QString percent = QString("%1%").arg(qRound(100.0 * span));

        painter.setBrush(borderColor);
        painter.setPen(this->palette().text().color());

        QPoint labelCenter = center + rotatedPoint(35, (from + span / 2) * 360);
        int fontHeight = painter.font().pointSize();
        QRect absoluteRect(labelCenter.x() - 35, labelCenter.y() - fontHeight, 80, 2 * fontHeight);
        painter.drawText(absoluteRect, Qt::AlignCenter, percent);
    }
}


/*! Draws the legend with free/used capacity
 *
 * \param painter the paint device
 * \param free    free space (in bytes) on the medium
 * \param span    used space (in bytes) on the medium
 */
void DriveCapacityUI::drawLegend(QPainter &painter, qint64 free, qint64 used)
{
    QRect freeRect(m_legendRect.left() + 10, m_legendRect.top() + 1, 10, 10);
    QRect usedRect(m_legendRect.left() + 10, m_legendRect.top() + 22, 10, 10);

    painter.setPen(this->palette().text().color());
    painter.setBrush(m_freeColor);
    painter.drawRect(freeRect);
    painter.setBrush(m_usedColor);
    painter.drawRect(usedRect);

    painter.drawText(freeRect.bottomLeft() + QPoint(15, 1), ScaleToSiPrefix(free) + " " + tr("free"));
    painter.drawText(usedRect.bottomLeft() + QPoint(15, 1), ScaleToSiPrefix(used) + " " + tr("used"));
}


/*! \copydoc QWidget::paintEvent(QPaintEvent *)
 */
void DriveCapacityUI::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setBackgroundMode(Qt::TransparentMode);

    if (m_model->freeCapacity() == 0 && m_model->usedCapacity() == 0) {
        m_lightFreeColor = this->palette().color(QPalette::Normal, QPalette::Light);
        m_lightUsedColor =  m_lightFreeColor;
        m_freeColor =  this->palette().color(QPalette::Normal, QPalette::Window);
        m_usedColor =  m_freeColor;
    } else {
        m_lightFreeColor = QColor(200, 250, 200);
        m_lightUsedColor = QColor(250, 200, 200);
        m_freeColor = QColor(0, 235, 100);
        m_usedColor = QColor(235, 0, 0);
    }

    m_pieChartRect.setRect(11, 11, 120, 120);
    m_legendRect.setRect(130, 15, 100, 100);

    qreal capacity = m_model->capacity();
    drawElement(painter, 0, capacity, m_lightFreeColor, m_freeColor);
    drawElement(painter, capacity, 1.0 - capacity, m_lightUsedColor, m_usedColor);

    drawLegend(painter, m_model->freeCapacity(), m_model->usedCapacity());
}
