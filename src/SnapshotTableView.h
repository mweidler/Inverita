/*
 * SnapshotTableView.hpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2014  Marc Weidler <marc.weidler@web.de>,
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

#ifndef HEADER_SNAPSHOTTABLEVIEW_INC
#define HEADER_SNAPSHOTTABLEVIEW_INC

#include <QTableView>


/*! Snapshot table visualization. Shows a list of snapshot.
 */
class SnapshotTableView : public QTableView
{
    Q_OBJECT

public:
    SnapshotTableView(QWidget *parent = 0);
    ~SnapshotTableView();

signals:
    void selectionChanged();

public slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif
