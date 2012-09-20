/*
 * ConfigurationDialog.h
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

#ifndef HEADER_CONFIGURATIONDIALOG_INC
#define HEADER_CONFIGURATIONDIALOG_INC

#include "Configuration.h"

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>


/*! Configuration dialog for setting various backup parameters, like
 *  - location and label
 *  - includes and exludes
 *  - options
 */
class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigurationDialog(Configuration &model, QWidget *parent);
    ~ConfigurationDialog();

    QString location() const;
    void setLocation(const QString &location);
    QString label() const;
    void setLabel(const QString &label);
    bool encrypt() const;
    void setEnableLocationChange(bool enable);
    void setEnableEncryptionSelection(bool enable);
    void setEncrypt(bool encrypt);

public slots:
    void onChangeButton();
    void onSave();

private:
    QWidget *constructStorageTab();
    QWidget *constructIncludesTab();
    QWidget *constructExcludesTab();
    QWidget *constructOptionsTab();

private:
    QCheckBox     *m_encryptBackup;
    QCheckBox     *m_verifyAfterBackup;
    QCheckBox     *m_verifyDigest;
    QCheckBox     *m_purgeBackups;
    QSpinBox      *m_spareCapacity;
    QCheckBox     *m_limitBackups;
    QSpinBox      *m_numberBackups;
    QLabel        *m_storageLocation;
    QLineEdit     *m_storageLabelEdit;
    QPushButton   *m_buttonChange;
    Configuration &m_config;
};

#endif
