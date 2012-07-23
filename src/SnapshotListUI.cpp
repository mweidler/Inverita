/*
 * SnapshotListUI.cpp
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

#include "SnapshotListUI.h"

#include <QLabel>
#include <QHeaderView>
#include <QBoxLayout>
#include <QDateTime>
#include <QDebug>

SnapshotListUI::SnapshotListUI(QAbstractTableModel *model, QWidget *parent) : QFrame(parent)
{
    QLabel *description = new QLabel(tr("Backup snapshots"));

    m_tableView = new SnapshotTableView();

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
    m_tableView->horizontalHeader()->setResizeMode(4, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setResizeMode(5, QHeaderView::Stretch);
    m_tableView->show();

    m_buttonValidate = new QPushButton(tr("Validate"));
    m_buttonValidate->setIcon(QIcon::fromTheme("dialog-yes"));
    m_buttonValidate->setEnabled(false);
    m_buttonDelete = new QPushButton(tr("Delete"));
    m_buttonDelete->setIcon(QIcon::fromTheme("edit-delete"));
    m_buttonDelete->setEnabled(false);
    m_buttonReload = new QPushButton(tr("Reload"));
    m_buttonReload->setIcon(QIcon::fromTheme("reload"));
    m_buttonReload->setEnabled(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setAlignment(Qt::AlignRight);
    buttonLayout->setMargin(0);
    buttonLayout->addWidget(m_buttonValidate);
    buttonLayout->addWidget(m_buttonDelete);
    buttonLayout->addWidget(m_buttonReload);

    QWidget *modificationButtons = new QWidget();
    modificationButtons->setLayout(buttonLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(description);
    mainLayout->addWidget(m_tableView);
    mainLayout->addWidget(modificationButtons);
    this->setLayout(mainLayout);

    setFrameStyle(QFrame::StyledPanel);

    connect(m_tableView, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(m_tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemSelected()));
    connect(m_buttonValidate, SIGNAL(clicked()), this, SLOT(onValidate()));
    connect(m_buttonDelete, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(m_buttonReload, SIGNAL(clicked()), this, SIGNAL(reload()));
}

SnapshotListUI::~SnapshotListUI()
{

}


void SnapshotListUI::setEnableReload(bool enable)
{
    m_buttonReload->setEnabled(enable);
}


int SnapshotListUI::currentSelection()
{
    return m_tableView->currentIndex().row();
}


void SnapshotListUI::onSelectionChanged()
{
    m_buttonValidate->setEnabled(false);
    m_buttonDelete->setEnabled(false);
}


void SnapshotListUI::onItemSelected()
{
    m_buttonValidate->setEnabled(true);
    m_buttonDelete->setEnabled(true);
}


void SnapshotListUI::onValidate()
{
    emit validateBackup();
    m_buttonValidate->setEnabled(false);
    m_buttonDelete->setEnabled(false);
}


void SnapshotListUI::onDelete()
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
