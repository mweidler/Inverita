/*
 * ConfigurationListUI.cpp
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

#include "ConfigurationListUI.h"

#include <QInputDialog>
#include <QLabel>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFileDialog>


ConfigurationListUI::ConfigurationListUI(QStringList &list, ConfigurationListMode mode, QString &description, QWidget *parent) : QWidget(parent), m_list(list)
{
    QLabel *descriptionLabel = new QLabel(description);
    descriptionLabel->setWordWrap(true);

    m_listModel = new QStringListModel(list);

    m_buttonAdd = new QPushButton(tr("Add Item"));
    m_buttonAdd->setIcon(QIcon::fromTheme("edit-add"));

    m_buttonRemove = new QPushButton(tr("Remove Item"));
    m_buttonRemove->setIcon(QIcon::fromTheme("edit-delete"));
    m_buttonRemove->setEnabled(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setAlignment(Qt::AlignRight);
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing(10);
    buttonLayout->addWidget(m_buttonAdd);
    buttonLayout->addWidget(m_buttonRemove);

    QWidget *modificationButtons = new QWidget();
    modificationButtons->setLayout(buttonLayout);

    m_listView = new QListView();
    m_listView->setModel(m_listModel);
    QVBoxLayout *listLayout = new QVBoxLayout;
    listLayout->setMargin(10);
    listLayout->setSpacing(20);
    listLayout->addWidget(descriptionLabel);
    listLayout->addWidget(m_listView);
    listLayout->addWidget(modificationButtons);
    this->setLayout(listLayout);

    connect(m_listView, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemSeleced()));
    if (mode == DIRECTORY) {
        connect(m_buttonAdd, SIGNAL(clicked()), this, SLOT(onAddDirectory()));
    } else {
        connect(m_buttonAdd, SIGNAL(clicked()), this, SLOT(onAddPattern()));
    }

    connect(m_buttonRemove, SIGNAL(clicked()), this, SLOT(onRemove()));
}


ConfigurationListUI::~ConfigurationListUI()
{

}


void ConfigurationListUI::onItemSeleced()
{
    m_buttonRemove->setEnabled(true);
}


void ConfigurationListUI::onAddDirectory()
{
    QFileDialog filedialog(this);
    filedialog.setFileMode(QFileDialog::Directory);
    filedialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    QStringList fileNames = filedialog.selectedFiles();
    int count = m_listModel->rowCount();
    m_listModel->insertRow(count);
    QModelIndex index = m_listModel->index(count);
    m_listModel->setData(index, fileNames.at(0));
    m_list = m_listModel->stringList();
}


void ConfigurationListUI::onAddPattern()
{
    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle(tr("Exclude pattern"));
    inputDialog.setLabelText(tr("Please enter a glob pattern, like 'Work?.txt', '*.txt', 'Trash', or 'lost+found', "
                                "where '?' matches one character and '*' matches any number of characters.<br><br>"
                                "Any file or directory that matches one of the patterns "
                                "will be excluded from the backup.") + "<br>");
    inputDialog.setMinimumSize(400, 200);
    inputDialog.resize(400, 200);
    if (inputDialog.exec() == QDialog::Rejected) {
        return;
    }

    QString pattern = inputDialog.textValue();
    if (pattern.isEmpty()) {
        return;
    }

    int count = m_listModel->rowCount();
    m_listModel->insertRow(count);
    QModelIndex index = m_listModel->index(count);
    m_listModel->setData(index, pattern);
    m_list = m_listModel->stringList();
}


void ConfigurationListUI::onRemove()
{
    m_listModel->removeRow(m_listView->currentIndex().row());
    m_list = m_listModel->stringList();
}

