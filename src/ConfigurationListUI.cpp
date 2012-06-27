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


ConfigurationListUI::ConfigurationListUI(QStringList &list, ConfigurationListMode mode, QString &description, QWidget *parent) : QWidget(parent), m_list(list)
{
    QLabel *descriptionLabel = new QLabel(description);
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
    m_listLayout = new QVBoxLayout;
    m_listLayout->setMargin(10);
    m_listLayout->setSpacing(20);
    m_listLayout->addWidget(descriptionLabel);
    m_listLayout->addWidget(m_listView);
    m_listLayout->addWidget(modificationButtons);
    this->setLayout(m_listLayout);

    connect(m_listView, SIGNAL(clicked(QModelIndex)), this, SLOT(OnItemSeleced()));
    if (mode == DIRECTORY) {
        connect(m_buttonAdd, SIGNAL(clicked()), this, SLOT(OnAddDirectory()));
    } else {
        connect(m_buttonAdd, SIGNAL(clicked()), this, SLOT(OnAddPattern()));
    }

    connect(m_buttonRemove, SIGNAL(clicked()), this, SLOT(OnRemove()));
}

ConfigurationListUI::~ConfigurationListUI()
{

}

void ConfigurationListUI::OnItemSeleced()
{
    m_buttonRemove->setEnabled(true);
}

void ConfigurationListUI::OnAddDirectory()
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

void ConfigurationListUI::OnAddPattern()
{
    QInputDialog inputdialog(this);
    inputdialog.setLabelText(tr("Please enter a exclude pattern"));
    inputdialog.setMinimumSize(640, 480);
    if (inputdialog.exec() == QDialog::Rejected) {
        return;
    }

    QString pattern = inputdialog.textValue();
    int count = m_listModel->rowCount();
    m_listModel->insertRow(count);
    QModelIndex index = m_listModel->index(count);
    m_listModel->setData(index, pattern);
    m_list = m_listModel->stringList();
}

void ConfigurationListUI::OnRemove()
{
    m_listModel->removeRow(m_listView->currentIndex().row());
    m_list = m_listModel->stringList();
    fprintf(stderr, "Debug: %d\n", m_list.size());
}

#if 0
void ConfigurationUI::OnIncludeSelected(wxListEvent &event)
{
    m_selectedIncludeItemIdx = event.GetIndex();
    buttonRemoveInclude->Enable();
}

void ConfigurationUI::OnIncludeDeSelected(wxListEvent &WXUNUSED(event))
{
    buttonRemoveInclude->Disable();
}

void ConfigurationUI::OnExcludeSelected(wxListEvent &event)
{
    m_selectedExcludeItemIdx = event.GetIndex();
    buttonRemoveExclude->Enable();
}

void ConfigurationUI::OnExcludeDeSelected(wxListEvent &WXUNUSED(event))
{
    buttonRemoveExclude->Disable();
}

void ConfigurationUI::OnIncludeEdited(wxListEvent &WXUNUSED(event))
{
    m_includesList->SetColumnWidth(0, m_includesList->GetClientSize().GetWidth());
}

void ConfigurationUI::OnExcludeEdited(wxListEvent &WXUNUSED(event))
{
    m_excludesList->SetColumnWidth(0, m_includesList->GetClientSize().GetWidth());
}

void ConfigurationUI::OnAddInclude(wxCommandEvent &WXUNUSED(event))
{
    wxDirDialog dialog(this, _("Choose a directory"), _("/"), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK) {
        m_config.GetIncludes().Add(dialog.GetPath());
        m_includesList->InsertItem(m_excludesList->GetItemCount(), dialog.GetPath());
    }
}

void ConfigurationUI::OnAddExclude(wxCommandEvent &WXUNUSED(event))
{
    wxTextEntryDialog dialog(this, _("Please enter a file pattern to be excluded,\ne.g. */lost+found/*"), _("Exclude pattern"));

    if (dialog.ShowModal() == wxID_OK) {
        m_config.GetExcludes().Add(dialog.GetValue());
        m_excludesList->InsertItem(m_excludesList->GetItemCount(), dialog.GetValue());
    }
}

void ConfigurationUI::OnRemoveInclude(wxCommandEvent &WXUNUSED(event))
{
    m_config.GetIncludes().RemoveAt(m_selectedIncludeItemIdx);
    m_includesList->DeleteItem(m_selectedIncludeItemIdx);
}

void ConfigurationUI::OnRemoveExclude(wxCommandEvent &WXUNUSED(event))
{
    m_config.GetExcludes().RemoveAt(m_selectedExcludeItemIdx);
    m_excludesList->DeleteItem(m_selectedExcludeItemIdx);
}

void ConfigurationUI::OnVerifyToggeled(wxCommandEvent &WXUNUSED(event))
{
    if (m_verify->IsChecked()) {
        m_verifyHash->Enable();
        m_verifyDate->Enable();
        m_verifySize->Enable();
    } else {
        m_verifyHash->Disable();
        m_verifyDate->Disable();
        m_verifySize->Disable();
    }
}

void ConfigurationUI::OnSave(wxCommandEvent &WXUNUSED(event))
{
    int verification = 0;

    if (m_verify->GetValue()) {
        verification |= VERIFY_ENABLED;
    }
    if (m_verifyHash->GetValue()) {
        verification |= VERIFY_CONTENT;
    }
    if (m_verifySize->GetValue()) {
        verification |= VERIFY_SIZE;
    }
    if (m_verifyDate->GetValue()) {
        verification |= VERIFY_TIME;
    }

    m_config.SetVerification(verification);
    m_config.SetBackupPurgeAllowed(m_purgeBackups->GetValue());
    m_config.SetBackupRestricted(m_limitBackups->GetValue());
    m_config.SetBackupRestriction(m_numberBackups->GetValue());

    EndModal(wxID_SAVE);
    Close(TRUE);
}
#endif
