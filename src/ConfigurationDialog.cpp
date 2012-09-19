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

#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>


/*! Constructs a new configuration dialog object.
 */
ConfigurationDialog::ConfigurationDialog(Configuration &model, QWidget *parent) : QDialog(parent), m_config(model)
{
    QTabWidget *tabWidget = new QTabWidget(parent);
    tabWidget->addTab(constructStorageTab(), tr("Storage"));
    tabWidget->addTab(constructIncludesTab(), tr("Includes"));
    tabWidget->addTab(constructExcludesTab(), tr("Excludes"));
    tabWidget->addTab(constructOptionsTab(), tr("Options"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Save);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Save)->setIcon(QIcon::fromTheme("filesave"));
    buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon::fromTheme("stop"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    layout->addWidget(tabWidget);
    layout->addWidget(buttonBox);
    this->setLayout(layout);

    buttonBox->button(QDialogButtonBox::Save)->setAutoDefault(true);
    buttonBox->button(QDialogButtonBox::Save)->setDefault(true);
    m_verifyAfterBackup->setChecked(m_config.verifyAfterBackup());
    m_verifyDigest->setChecked(m_config.verifyDigest());
    m_purgeBackups->setChecked(m_config.autoDeleteBackups());
    m_spareCapacity->setValue(m_config.spareCapacity());
    m_limitBackups->setChecked(m_config.limitBackups());
    m_numberBackups->setValue(m_config.maximumBackups());

    connect(m_buttonChange, SIGNAL(clicked()), this, SLOT(onChangeButton()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(onSave()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setMinimumSize(650, 500);
    resize(650, 500);
}


/*! Destructor
 */
ConfigurationDialog::~ConfigurationDialog()
{

}


/*! Construct the configuration tab for storage configuration.
 */
QWidget *ConfigurationDialog::constructStorageTab()
{
    QLabel *storageText = new QLabel(
        tr(
            "The backup storage specifies the location, where backup "
            "data will be stored. You can choose every pathname which is "
            "accessible from your computer, e.g. '/media/usbdrive' or "
            "'/data/backup'<br><br>"
            "<u>Attention:</u> You can not change the backup storage after "
            "once created. The encryption can only be enabled/disabeled "
            "if no backup snapshots are currently available."
        ));
    storageText->setWordWrap(true);

    QPixmap pixmap(":/images/drive-icon.png");
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(pixmap);

    QHBoxLayout *descriptionLayout = new QHBoxLayout;
    descriptionLayout->addWidget(storageText, 1);
    descriptionLayout->addWidget(labelImage, 0, Qt::AlignRight | Qt::AlignTop);

    QHBoxLayout *locationLayout = new QHBoxLayout;
    m_storageEdit = new QLineEdit();
    m_buttonChange = new QPushButton(tr("Change"));
    m_buttonChange->setIcon(QIcon::fromTheme("fileopen"));
    locationLayout->addWidget(m_storageEdit);
    locationLayout->addWidget(m_buttonChange);

    m_encryptBackup = new QCheckBox(tr("Encrypt backup data using 'EncFS'"));

    QVBoxLayout *storageLayout = new QVBoxLayout;
    storageLayout->addWidget(new QLabel(tr("Current backup storage:")));
    storageLayout->addLayout(locationLayout);
    storageLayout->addWidget(m_encryptBackup);

    QLabel *separator = new QLabel;
    separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QLabel *labelText = new QLabel(tr("You can give your backup a short label, like 'Home-Backup' or 'Data'."));

    QHBoxLayout *labelLayout = new QHBoxLayout;
    m_storageLabelEdit = new QLineEdit();
    labelLayout->addWidget(new QLabel(tr("Backup label:")));
    labelLayout->addWidget(m_storageLabelEdit);

    QVBoxLayout *storageVLayout = new QVBoxLayout;
    storageVLayout->addLayout(descriptionLayout);
    storageVLayout->addSpacerItem(new QSpacerItem(0, 20));
    storageVLayout->addLayout(storageLayout);
    storageVLayout->addWidget(separator);
    storageVLayout->addWidget(labelText);
    storageVLayout->addLayout(labelLayout);

    QWidget *pageStorage = new QWidget();
    pageStorage->setLayout(storageVLayout);

    return pageStorage;
}


/*! Construct the configuration tab for includes configuration.
 */
QWidget *ConfigurationDialog::constructIncludesTab()
{
    QString description = tr("The list below shows all directories that comprises your backup, "
                             "e.g. '/data' or '/home/user'");

    return new ConfigurationListUI(m_config.includes(), ConfigurationListUI::DIRECTORY, description, this);
}


/*! Construct the configuration tab for excludes configuration.
 */
QWidget *ConfigurationDialog::constructExcludesTab()
{
    QString description = tr("The list below shows all file/directory-patterns that will be excluded "
                             "from the backup, e.g. '/data/temp' will exclude the whole directory, "
                             "'test*' will exclude all items beginning with 'test'");

    return new ConfigurationListUI(m_config.excludes(), ConfigurationListUI::PATTERN, description, this);
}


/*! Construct the configuration tab for options configuration.
 */
QWidget *ConfigurationDialog::constructOptionsTab()
{
    QLabel *separator1 = new QLabel;
    QLabel *separator2 = new QLabel;
    QLabel *separator3 = new QLabel;
    separator1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    separator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    separator3->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QLabel *verifyText = new QLabel(tr("Backup snapshots can be verified after creation or on individual request."));
    verifyText->setWordWrap(true);
    m_verifyAfterBackup = new QCheckBox(tr("Verify backup snapshots automatically after creation"));
    QLabel *digestText = new QLabel(tr("Backup snapshot verification ensures the consistency of the digests to "
                                       "the corresponding files, and ensures that all files exist. For performance "
                                       "reasons, the digest recomputation can be left out."));
    digestText->setWordWrap(true);
    m_verifyDigest = new QCheckBox(tr("Recompute digests on verification (recommended)"));

    QLabel *purgeText = new QLabel(tr("At low drive space, the oldest backup snapshots can be deleted automatically."));
    purgeText->setWordWrap(true);
    QHBoxLayout *purgeLayout = new QHBoxLayout;
    m_purgeBackups = new QCheckBox(tr("Delete oldest backup snapshots until free capacity reaches"));
    m_spareCapacity = new QSpinBox;
    m_spareCapacity->setMaximum(15);
    m_spareCapacity->setMinimum(5);
    QLabel *purgeTrailingText = new QLabel(tr("percent"));
    purgeLayout->setAlignment(Qt::AlignLeft);
    purgeLayout->addWidget(m_purgeBackups);
    purgeLayout->addWidget(m_spareCapacity);
    purgeLayout->addWidget(purgeTrailingText);

    QLabel *limitText = new QLabel(tr("The number of backup snapshots can be limited."));
    QHBoxLayout *limitLayout = new QHBoxLayout;
    m_limitBackups = new QCheckBox(tr("Keep up to"));
    m_numberBackups = new QSpinBox;
    m_numberBackups->setMaximum(50);
    m_numberBackups->setMinimum(1);
    QLabel *trailingText = new QLabel(tr("backup snapshots and delete the oldest ones"));
    limitLayout->setAlignment(Qt::AlignLeft);
    limitLayout->addWidget(m_limitBackups);
    limitLayout->addWidget(m_numberBackups);
    limitLayout->addWidget(trailingText);

    QVBoxLayout *optionsLayout = new QVBoxLayout;
    optionsLayout->addWidget(verifyText);
    optionsLayout->addWidget(m_verifyAfterBackup);
    optionsLayout->addWidget(separator1);
    optionsLayout->addWidget(digestText);
    optionsLayout->addWidget(m_verifyDigest);
    optionsLayout->addWidget(separator2);
    optionsLayout->addWidget(purgeText);
    optionsLayout->addLayout(purgeLayout);
    optionsLayout->addWidget(separator3);
    optionsLayout->addWidget(limitText);
    optionsLayout->addLayout(limitLayout);

    QWidget *pageOptions = new QWidget;
    pageOptions->setLayout(optionsLayout);

    return pageOptions;
}


/*! \returns the location entered in the location edit field.
 */
QString ConfigurationDialog::location() const
{
    return m_storageEdit->text();
}


/*! Sets the given location to the location edit field.
 *
 * \param location the new location to be set into the edit field
 */
void ConfigurationDialog::setLocation(const QString &location)
{
    m_storageEdit->setText(location);
}


/*! \returns the label entered in the label edit field.
 */
QString ConfigurationDialog::label() const
{
    return m_storageLabelEdit->text();
}


/*! Sets the given label to the label edit field.
 *
 * \param label the new label to be set into the edit field
 */
void ConfigurationDialog::setLabel(const QString &label)
{
    m_storageLabelEdit->setText(label);
}


bool ConfigurationDialog::encrypt() const
{
    return m_encryptBackup->isChecked();
}


void ConfigurationDialog::setEnableLocationChange(bool enable)
{
    m_storageEdit->setReadOnly(!enable);
    m_buttonChange->setEnabled(enable);
}


void ConfigurationDialog::setEnableEncryptionSelection(bool enable)
{
    m_encryptBackup->setEnabled(enable);
}


void ConfigurationDialog::setEncrypt(bool encrypt)
{
    m_encryptBackup->setChecked(encrypt);
}


/*! Is called when the user presses the change "location button" on the storage tab.
 */
void ConfigurationDialog::onChangeButton()
{
    QFileDialog filedialog(this);
    filedialog.setWindowTitle(tr("Select a new backup location..."));
    filedialog.setFileMode(QFileDialog::Directory);
    filedialog.setDirectory(m_storageEdit->text());
    filedialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (filedialog.exec() == QDialog::Rejected) {
        return;
    }

    QString dirname = filedialog.selectedFiles()[0];
    m_storageEdit->setText(dirname);
}


/*! Is called when the user presses the change "Save button" of this dialog.
 */
void ConfigurationDialog::onSave()
{
    if (!QFile::exists(location())) {
        QMessageBox::critical(this,
                              tr("Backup storage does not exist"),
                              tr("The backup storage you have specified does not exist.<br>"
                                 "Please choose another storage.")
                             );
        return;
    }

    m_config.setVerifyAfterBackup(m_verifyAfterBackup->isChecked());
    m_config.setVerifyDigest(m_verifyDigest->isChecked());
    m_config.setAutoDeleteBackups(m_purgeBackups->isChecked());
    m_config.setLimitBackups(m_limitBackups->isChecked());
    m_config.setMaximumBackups(m_numberBackups->value());
    m_config.setSpareCapacity(m_spareCapacity->value());

    accept();
}
