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

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>

#include "Configuration.h"


class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigurationDialog(Configuration &model, QWidget *parent);
    ~ConfigurationDialog();

    QString location() const;
    void setLocation(const QString &location);

public slots:
    void onChangeButton();
    void onVerifyToggeled();
    void onSave();

private:
    QCheckBox     *m_verify;
    QCheckBox     *m_verifyHash;
    QCheckBox     *m_verifyDate;
    QCheckBox     *m_verifySize;
    QCheckBox     *m_purgeBackups;
    QCheckBox     *m_limitBackups;
    QSpinBox      *m_numberBackups;
    QLineEdit     *m_targetEdit;
    QPushButton   *m_buttonChange;
    Configuration &m_config;
};

#endif
