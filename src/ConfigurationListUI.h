/*
 * ConfigurationListUI.h
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

#ifndef HEADER_CONFIGURATIONLISTUI_INC
#define HEADER_CONFIGURATIONLISTUI_INC

#include <QStringListModel>
#include <QListView>
#include <QPushButton>


/*! Configuration dialog for adding includes/excludes to a list
 */
class ConfigurationListUI : public QWidget
{
    Q_OBJECT

public:
    enum ConfigurationListMode {
        DIRECTORY,
        PATTERN
    };

    ConfigurationListUI(QStringList &list, ConfigurationListMode mode, QString &description, QWidget *parent = 0);
    ~ConfigurationListUI();

public slots:
    void onItemSeleced();
    void onAddDirectory();
    void onAddPattern();
    void onRemove();

private:
    QStringList      &m_list;
    QStringListModel *m_listModel;
    QListView        *m_listView;
    QPushButton      *m_buttonAdd;
    QPushButton      *m_buttonRemove;
};

#endif
