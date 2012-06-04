/*
 * BackupHistoryUI.cpp
 *
 * Copyright (c) 2012, Marc Weidler <marc.weidler@web.de>
 * Ulrichstr. 12/1, 71672 Marbach, Germany
 * All rights reserved.
 *
 * 'ALVARA Personal Backup' is licensed under the BSD license (simplified, 2-clause).
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
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
    //tableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
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
    connect(m_buttonValidate, SIGNAL(clicked()), this, SIGNAL(validateBackup()));
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
