/*
 * BackupSelectorUI.h
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

#ifndef HEADER_BACKUPSELECTORUI_INC
#define HEADER_BACKUPSELECTORUI_INC

#include <QtGui>
#include "BackupListModel.h"


/*!
 * Create a new BackupSelectorUI component with GUI elements
 */
class BackupSelectorUI : public QWidget
{
    Q_OBJECT

public:
    BackupSelectorUI(QAbstractListModel *model, QWidget *parent = 0);
    ~BackupSelectorUI();

    int currentSelection();

signals:
    void backupSelected();

protected:
    void appendChoiceUnique(const QString &filepath);

protected slots:
    void onSelect();
    void onNew();
    void onConfigure();

protected:
    QComboBox *m_choice;
};

#endif
