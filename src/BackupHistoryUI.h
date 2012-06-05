/**
 * BackupHistoryUI.hpp
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

#ifndef HEADER_BACKUPHISTORYUI_INC
#define HEADER_BACKUPHISTORYUI_INC

#include <QtGui>

class BackupHistoryUI : public QFrame
{
    Q_OBJECT

public:
    BackupHistoryUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~BackupHistoryUI();

    int currentSelection();

signals:
    void validateBackup();
    void deleteBackup();
    void reload();

public slots:
    void OnItemSelected();
    void OnValidate();
    void OnDelete();

protected:
    QTableView  *m_tableView;
    QPushButton *m_buttonValidate;
    QPushButton *m_buttonDelete;
    QPushButton *m_buttonReload;
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
