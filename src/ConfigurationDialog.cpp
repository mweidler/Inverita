/*
 * ConfigurationDialog.cpp
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

#include <QtCore>
#include <QtGui>
#include <QString>
#include <QStringList>
#include <QDialog>
#include <QLabel>
#include <QTableWidget>

#include "ConfigurationDialog.h"
#include "ConfigurationListUI.h"

ConfigurationDialog::ConfigurationDialog(Configuration &model, QWidget *parent) : QDialog(parent), m_config(model)
{
    QString includeText = tr("The list below defines the coverage of your backup.\n"
                             "You can specify any many files, file patterns or directories you want.");
    QString excludeText = tr("The patterns below specify all files/directories that will"
                             "be excluded from the backup");

    ConfigurationListUI *includeListUI = new ConfigurationListUI(m_config.GetIncludes(), ConfigurationListUI::DIRECTORY, includeText, this);
    ConfigurationListUI *excludeListUI = new ConfigurationListUI(m_config.GetExcludes(), ConfigurationListUI::PATTERN,   excludeText, this);

    QLabel *seperator1 = new QLabel;
    QLabel *seperator2 = new QLabel;
    seperator1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    seperator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QLabel *verifyText = new QLabel(tr("Backups can be verified after backup or on individual request."));
    m_verify = new QCheckBox(tr("&Verify backup with"));

    QVBoxLayout *verifyLayout = new QVBoxLayout;
    verifyLayout->setContentsMargins(50, 0, 0, 0);
    m_verifyHash = new QCheckBox(tr("SHA1 check sum"));
    m_verifyDate = new QCheckBox(tr("Date"));
    m_verifySize = new QCheckBox(tr("File size"));
    verifyLayout->addWidget(m_verifyHash);
    verifyLayout->addWidget(m_verifyDate);
    verifyLayout->addWidget(m_verifySize);

    QLabel *purgeText = new QLabel(tr("On low drive space, the oldest backups can be deleted automatically."));
    m_purgeBackups = new QCheckBox(tr("Delete oldest backup(s) on low drive space"));

    QLabel *limitText = new QLabel(tr("The number of historical backups can be limited."));
    QHBoxLayout *limitLayout = new QHBoxLayout;
    m_limitBackups = new QCheckBox(tr("Keep up to"));
    m_numberBackups = new QSpinBox;
    m_numberBackups->setMaximum(50);
    m_numberBackups->setMinimum(1);
    QLabel *trailingText = new QLabel(tr("previous backups"));
    limitLayout->setAlignment(Qt::AlignLeft);
    limitLayout->addWidget(m_limitBackups);
    limitLayout->addWidget(m_numberBackups);
    limitLayout->addWidget(trailingText);

    QWidget *pageOptions = new QWidget;
    QVBoxLayout *optionsLayout = new QVBoxLayout;
    optionsLayout->addWidget(verifyText);
    optionsLayout->addWidget(m_verify);
    optionsLayout->addLayout(verifyLayout);
    optionsLayout->addWidget(seperator1);
    optionsLayout->addWidget(purgeText);
    optionsLayout->addWidget(m_purgeBackups);
    optionsLayout->addWidget(seperator2);
    optionsLayout->addWidget(limitText);
    optionsLayout->addLayout(limitLayout);
    pageOptions->setLayout(optionsLayout);

    // create the final tab
    QTabWidget *tabWidget = new QTabWidget(parent);
    tabWidget->addTab(includeListUI, tr("Includes"));
    tabWidget->addTab(excludeListUI, tr("Excludes"));
    tabWidget->addTab(pageOptions, tr("Options"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Save);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Save)->setIcon(QIcon::fromTheme("ok"));
    buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon::fromTheme("stop"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    layout->addWidget(tabWidget);
    layout->addWidget(buttonBox);
    this->setLayout(layout);

    connect(m_verify, SIGNAL(toggled(bool)), this, SLOT(onVerifyToggeled()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(onSave()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    if (m_config.GetVerification() & VERIFY_ENABLED) {
        m_verify->setChecked(true);
    }

    if (m_config.GetVerification() & VERIFY_CONTENT) {
        m_verifyHash->setChecked(true);
    }

    if (m_config.GetVerification() & VERIFY_TIME) {
        m_verifyDate->setChecked(true);
    }

    if (m_config.GetVerification() & VERIFY_SIZE) {
        m_verifySize->setChecked(true);
    }

    m_purgeBackups->setChecked(m_config.GetBackupPurgeAllowed());
    m_limitBackups->setChecked(m_config.GetBackupRestricted());
    m_numberBackups->setValue(m_config.GetBackupRestriction());

    setMinimumSize(640, 480);
    resize(640, 480);
    setWindowTitle(tr("Backup configuration"));
}

ConfigurationDialog::~ConfigurationDialog()
{

}

void ConfigurationDialog::onVerifyToggeled()
{
    if (m_verify->isChecked()) {
        m_verifyHash->setEnabled(true);
        m_verifyDate->setEnabled(true);
        m_verifySize->setEnabled(true);
    } else {
        m_verifyHash->setEnabled(false);
        m_verifyDate->setEnabled(false);
        m_verifySize->setEnabled(false);
    }
}

void ConfigurationDialog::onSave()
{
    int verification = 0;

    if (m_verify->isChecked()) {
        verification |= VERIFY_ENABLED;
    }
    if (m_verifyHash->isChecked()) {
        verification |= VERIFY_CONTENT;
    }
    if (m_verifySize->isChecked()) {
        verification |= VERIFY_SIZE;
    }
    if (m_verifyDate->isChecked()) {
        verification |= VERIFY_TIME;
    }

    m_config.SetVerification(verification);
    m_config.SetBackupPurgeAllowed(m_purgeBackups->isChecked());
    m_config.SetBackupRestricted(m_limitBackups->isChecked());
    m_config.SetBackupRestriction(m_numberBackups->value());

    accept();
}