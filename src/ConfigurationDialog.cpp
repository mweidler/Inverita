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


#include "ConfigurationDialog.h"
#include "ConfigurationListUI.h"


ConfigurationDialog::ConfigurationDialog(Configuration &model, QWidget *parent) : QDialog(parent), m_config(model)
{
    QTabWidget *tabWidget = new QTabWidget(parent);
    tabWidget->addTab(constructTargetTab(), tr("Target"));
    tabWidget->addTab(constructIncludesTab(), tr("Includes"));
    tabWidget->addTab(constructExcludesTab(), tr("Excludes"));
    tabWidget->addTab(constructOptionsTab(), tr("Options"));

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

    m_verifyAfterBackup->setChecked(m_config.verifyAfterBackup());
    m_verifyHash->setChecked(m_config.verifyHash());
    m_verifyDate->setChecked(m_config.verifyTime());
    m_verifySize->setChecked(m_config.verifySize());
    m_purgeBackups->setChecked(m_config.autoDeleteBackups());
    m_limitBackups->setChecked(m_config.limitBackups());
    m_numberBackups->setValue(m_config.maximumBackups());

    connect(m_buttonChange, SIGNAL(clicked()), this, SLOT(onChangeButton()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(onSave()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setMinimumSize(620, 480);
    resize(620, 480);
}

ConfigurationDialog::~ConfigurationDialog()
{

}


QWidget *ConfigurationDialog::constructTargetTab()
{
    QLabel *targetText = new QLabel(tr("The backup target specifies the location, where the backup<br>"
                                       "data will be stored on. This can be any path mounted on<br>"
                                       "your computer, e.g. <i>/media/usbdrive</i> or <i>/data/backup</i><br><br>"
                                       "<u>Attention:</u> If you change an existing backup location to<br>"
                                       " another location, a new backup will be created on the new<br>"
                                       "location. Your existing backup and snapshots will not be<br>"
                                       "copied to the new location and reside on the old location.<br><br><br>"
                                       "Your current backup target:"
                                      ));

    QPixmap pixmap(":/images/drive-icon.png");
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(pixmap);

    QHBoxLayout *descriptionLayout = new QHBoxLayout;
    descriptionLayout->addWidget(targetText);
    descriptionLayout->addWidget(labelImage, 1, Qt::AlignRight | Qt::AlignTop);

    QHBoxLayout *locationLayout = new QHBoxLayout;
    m_targetEdit = new QLineEdit();
    m_buttonChange = new QPushButton(tr("Change"));
    m_buttonChange->setIcon(QIcon::fromTheme("fileopen"));
    locationLayout->addWidget(m_targetEdit);
    locationLayout->addWidget(m_buttonChange);

    QLabel *seperator = new QLabel;
    seperator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QLabel *labelText = new QLabel(tr("Give your backup a short label, like 'home backup' or 'dropbox'."));

    QHBoxLayout *labelLayout = new QHBoxLayout;
    m_targetLabelEdit = new QLineEdit();
    labelLayout->addWidget(new QLabel(tr("Label:")));
    labelLayout->addWidget(m_targetLabelEdit);

    QWidget *pageTarget = new QWidget();
    QVBoxLayout *targetVLayout = new QVBoxLayout;
    targetVLayout->addLayout(descriptionLayout);
    targetVLayout->addLayout(locationLayout);
    targetVLayout->addWidget(seperator);
    targetVLayout->addWidget(labelText);
    targetVLayout->addLayout(labelLayout);
    pageTarget->setLayout(targetVLayout);

    return pageTarget;
}

QWidget *ConfigurationDialog::constructIncludesTab()
{
    QString includeText = tr("The list below defines the coverage of your backup. With each entry,<br>"
                             "you specify a directory that will be included in your backup,<br>"
                             "e.g. <i>/data</i> or <i>/home/user</i>");

    ConfigurationListUI *includeListUI = new ConfigurationListUI(m_config.GetIncludes(), ConfigurationListUI::DIRECTORY, includeText, this);

    return includeListUI;

}

QWidget *ConfigurationDialog::constructExcludesTab()
{
    QString excludeText = tr("The patterns below specify all files/directories that will be excluded from<br>"
                             "the backup, e.g. <i>/data/temp</i> will exclude the whole directrory,<br>"
                             "<i>test*</i> will exclude all files beginning with 'test'");

    ConfigurationListUI *excludeListUI = new ConfigurationListUI(m_config.GetExcludes(), ConfigurationListUI::PATTERN,   excludeText, this);

    return excludeListUI;
}


QWidget *ConfigurationDialog::constructOptionsTab()
{
    QLabel *seperator1 = new QLabel;
    QLabel *seperator2 = new QLabel;
    QLabel *seperator3 = new QLabel;
    seperator1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    seperator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    seperator3->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QLabel *verifyText = new QLabel(tr("Backups can be verified after backup or on individual request."));
    m_verifyAfterBackup = new QCheckBox(tr("&Verify snapshot after each new creation"));
    QLabel *verifyLabel = new QLabel(tr("On verification, verify backup snapshot including..."));

    QVBoxLayout *verifyLayout = new QVBoxLayout;
    verifyLayout->setContentsMargins(50, 0, 0, 0);
    m_verifyHash = new QCheckBox(tr("Content checksum (SHA1 hash)"));
    m_verifyDate = new QCheckBox(tr("Date and time"));
    m_verifySize = new QCheckBox(tr("File size"));
    verifyLayout->addWidget(m_verifyHash);
    verifyLayout->addWidget(m_verifyDate);
    verifyLayout->addWidget(m_verifySize);

    QLabel *purgeText = new QLabel(tr("On low drive space, the oldest backups can be deleted automatically."));
    m_purgeBackups = new QCheckBox(tr("&Delete oldest backups on low drive space"));

    QLabel *limitText = new QLabel(tr("The number of historical backups can be limited."));
    QHBoxLayout *limitLayout = new QHBoxLayout;
    m_limitBackups = new QCheckBox(tr("&Keep up to"));
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
    optionsLayout->addWidget(m_verifyAfterBackup);
    optionsLayout->addWidget(seperator1);
    optionsLayout->addWidget(verifyLabel);
    optionsLayout->addLayout(verifyLayout);
    optionsLayout->addWidget(seperator2);
    optionsLayout->addWidget(purgeText);
    optionsLayout->addWidget(m_purgeBackups);
    optionsLayout->addWidget(seperator3);
    optionsLayout->addWidget(limitText);
    optionsLayout->addLayout(limitLayout);
    pageOptions->setLayout(optionsLayout);

    return pageOptions;
}

QString ConfigurationDialog::location() const
{
    return m_targetEdit->text();
}

void ConfigurationDialog::setLocation(const QString &location)
{
    m_targetEdit->setText(location);
}

QString ConfigurationDialog::label() const
{
    return m_targetLabelEdit->text();
}

void ConfigurationDialog::setLabel(const QString &label)
{
    m_targetLabelEdit->setText(label);
}

void ConfigurationDialog::onChangeButton()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Select a new backup location..."));
    filedialog.setFileMode(QFileDialog::Directory);
    filedialog.setDirectory(m_targetEdit->text());
    filedialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    QString dirname = filedialog.selectedFiles()[0];
    m_targetEdit->setText(dirname);
}


void ConfigurationDialog::onSave()
{
    if (!QFile::exists(location())) {
        QMessageBox::critical(this,
                              tr("Backup target does not exist"),
                              tr("The backup target you have specified does not exist.\n"
                                 "Please choose another target location.")
                             );
        return;
    }

    m_config.setVerifyAfterBackup(m_verifyAfterBackup->isChecked());
    m_config.setVerifyHash(m_verifyHash->isChecked());
    m_config.setVerifySize(m_verifySize->isChecked());
    m_config.setVerifyTime(m_verifyDate->isChecked());
    m_config.setAutoDeleteBackups(m_purgeBackups->isChecked());
    m_config.setLimitBackups(m_limitBackups->isChecked());
    m_config.setMaximumBackups(m_numberBackups->value());

    accept();
}
