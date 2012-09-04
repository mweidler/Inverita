/*
 * SnapshotListUI.hpp
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

#ifndef HEADER_SNAPSHOTLISTUI_INC
#define HEADER_SNAPSHOTLISTUI_INC

#include "SnapshotTableView.h"
#include <QPushButton>
#include <QStyledItemDelegate>


class SnapshotListUI : public QFrame
{
    Q_OBJECT

public:
    SnapshotListUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~SnapshotListUI();

    void setEnableReload(bool enable);
    int currentSelection();

signals:
    void validateSnapshot();
    void deleteSnapshot();
    void reloadSnapshots();

public slots:
    void onSelectionChanged();
    void onItemSelected();
    void onValidate();
    void onDelete();
    void onReload();

protected:
    SnapshotTableView *m_tableView;
    QPushButton       *m_buttonValidate;
    QPushButton       *m_buttonDelete;
    QPushButton       *m_buttonReload;
};


class BackupHistoryUIDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    BackupHistoryUIDelegate(QWidget *parent = 0);
    ~BackupHistoryUIDelegate();

    QString displayText(const QVariant &value, const QLocale &locale) const;

protected:
};

#endif
