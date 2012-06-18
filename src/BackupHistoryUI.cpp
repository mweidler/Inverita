/*
 * BackupHistoryUI.cpp
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

#include <QtGui>
#include "BackupHistoryUI.h"

BackupHistoryUI::BackupHistoryUI(QAbstractTableModel *model, QWidget *parent) : QFrame(parent)
{
    QLabel *description = new QLabel(tr("Backup snapshots"));

    m_tableView = new QTableView();

    m_tableView->setModel(model);
    m_tableView->setItemDelegate(new BackupHistoryUIDelegate(this));
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setAlternatingRowColors(true);

    m_tableView->verticalHeader()->hide();
    m_tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_tableView->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setResizeMode(3, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setDefaultSectionSize(150);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->show();

    m_buttonValidate = new QPushButton(tr("Validate"));
    m_buttonValidate->setIcon(QIcon::fromTheme("edit-find"));
    m_buttonValidate->setEnabled(false);
    m_buttonDelete = new QPushButton(tr("Delete"));
    m_buttonDelete->setIcon(QIcon::fromTheme("edit-delete"));
    m_buttonDelete->setEnabled(false);
    m_buttonReload = new QPushButton(tr("Reload"));
    m_buttonReload->setIcon(QIcon::fromTheme("reload"));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setAlignment(Qt::AlignRight);
    //buttonLayout->setMargin(5);
    //buttonLayout->setSpacing(10);
    buttonLayout->addWidget(m_buttonValidate);
    buttonLayout->addWidget(m_buttonDelete);
    buttonLayout->addWidget(m_buttonReload);

    QWidget *modificationButtons = new QWidget();
    modificationButtons->setLayout(buttonLayout);

    QVBoxLayout *vboxlayout = new QVBoxLayout;
    vboxlayout->setMargin(5);
    vboxlayout->addWidget(description);
    vboxlayout->addWidget(m_tableView);
    vboxlayout->addWidget(modificationButtons);
    this->setLayout(vboxlayout);

    setFrameStyle(QFrame::StyledPanel);

    connect(m_tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(OnItemSelected()));
    connect(m_buttonValidate, SIGNAL(clicked()), this, SLOT(OnValidate()));
    connect(m_buttonDelete, SIGNAL(clicked()), this, SLOT(OnDelete()));
    connect(m_buttonReload, SIGNAL(clicked()), this, SIGNAL(reload()));
}

BackupHistoryUI::~BackupHistoryUI()
{

}

int BackupHistoryUI::currentSelection()
{
    return m_tableView->currentIndex().row();
}

void BackupHistoryUI::OnItemSelected()
{
    m_buttonValidate->setEnabled(true);
    m_buttonDelete->setEnabled(true);
    qDebug() << "Current index" << m_tableView->currentIndex();
}

void BackupHistoryUI::OnValidate()
{
    emit validateBackup();
    m_buttonValidate->setEnabled(false);
    m_buttonDelete->setEnabled(false);
}

void BackupHistoryUI::OnDelete()
{
    emit deleteBackup();
    m_buttonValidate->setEnabled(false);
    m_buttonDelete->setEnabled(false);
}

BackupHistoryUIDelegate::BackupHistoryUIDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{

}

BackupHistoryUIDelegate::~BackupHistoryUIDelegate()
{

}

QString BackupHistoryUIDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    switch (value.type()) {
        case QVariant::DateTime:
            return QString(value.toDateTime().toString(locale.dateTimeFormat()));
            break;

        case QVariant::Int:
            return locale.toString(value.toInt());
            break;

        default:
            return value.toString();
            break;
    }
}
