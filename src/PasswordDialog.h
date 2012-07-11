/*
 * PasswordDialog.h
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

#ifndef HEADER_PASSWORDDIALOG_INC
#define HEADER_PASSWORDDIALOG_INC

#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>


/*! Password edit dialog for encrypted backups
 */
class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    PasswordDialog(QWidget *parent);
    ~PasswordDialog();

    QString password() const;
    void setPassword(const QString &password);

    bool rememberPassword() const;
    void setRememberPassword(bool checked);

public slots:
    void adjustEchoMode();

private:
    QLineEdit *m_passwordEdit;
    QCheckBox *m_showPassword;
    QCheckBox *m_rememberPassword;
};

#endif
