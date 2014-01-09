/*
 * SnapshotListUI.cpp
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

#include "SnapshotListUI.h"

#include <QLabel>
#include <QHeaderView>
#include <QBoxLayout>
#include <QDateTime>


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

    m_buttonOpen = new QPushButton(tr("Open"));
    m_buttonOpen->setIcon(QIcon::fromTheme("folder-open"));
    m_buttonOpen->setEnabled(false);
    m_buttonValidate = new QPushButton(tr("Validate"));
    m_buttonValidate->setIcon(QIcon::fromTheme("edit-find"));
    m_buttonValidate->setEnabled(false);
    m_buttonDelete = new QPushButton(tr("Delete"));
    m_buttonDelete->setIcon(QIcon::fromTheme("edit-delete"));
    m_buttonDelete->setEnabled(false);
    m_buttonReload = new QPushButton(tr("Reload"));
    m_buttonReload->setIcon(QIcon::fromTheme("reload", QIcon(":/images/reload.png")));
    m_buttonReload->setEnabled(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setAlignment(Qt::AlignRight);
    buttonLayout->setMargin(0);
    buttonLayout->addWidget(m_buttonOpen);
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
    connect(m_buttonOpen, SIGNAL(clicked()), this, SLOT(onOpen()));
    connect(m_buttonValidate, SIGNAL(clicked()), this, SLOT(onValidate()));
    connect(m_buttonDelete, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(m_buttonReload, SIGNAL(clicked()), this, SLOT(onReload()));
}


SnapshotListUI::~SnapshotListUI()
{

}


/*! \copydoc QWidget::minimumSizeHint()
 */
QSize SnapshotListUI::minimumSizeHint() const
{
    return QSize(200, 250);
}


/*! \copydoc QWidget::sizeHint()
 */
QSize SnapshotListUI::sizeHint() const
{
    return minimumSizeHint();
}


void SnapshotListUI::setEnableModifiers(bool enable)
{
    m_buttonOpen->setEnabled(enable);
    m_buttonValidate->setEnabled(enable);
    m_buttonDelete->setEnabled(enable);
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
    setEnableModifiers(false);
}


void SnapshotListUI::onItemSelected()
{
    setEnableModifiers(true);
}


void SnapshotListUI::onOpen()
{
    emit openSnapshot();
}


void SnapshotListUI::onValidate()
{
    emit validateSnapshot();
}


void SnapshotListUI::onDelete()
{
    emit deleteSnapshot();
    setEnableModifiers(false);
}


void SnapshotListUI::onReload()
{
    emit reloadSnapshots();
    setEnableModifiers(false);
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
