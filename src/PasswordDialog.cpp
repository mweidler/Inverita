/*
 * PasswordDialog.cpp
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


#include "PasswordDialog.h"

#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>


/*! Constructs a new password dialog object.
 */
PasswordDialog::PasswordDialog(QWidget *parent) : QDialog(parent)
{
    QLabel *labelDescription =
        new QLabel(tr("The contents of this backup are encrypted using an individual password.<br>"
                      "Please enter this password to get access.")
                  );

    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(QIcon::fromTheme("dialog-password").pixmap(48, 48));

    QHBoxLayout *descriptionLayout = new QHBoxLayout;
    descriptionLayout->setAlignment(Qt::AlignLeft);
    descriptionLayout->addWidget(labelImage);
    descriptionLayout->addSpacerItem(new QSpacerItem(10, 0));
    descriptionLayout->addWidget(labelDescription);

    m_passwordEdit = new QLineEdit();
    m_showPassword = new QCheckBox(tr("Show password unmasked"));
    m_rememberPassword = new QCheckBox(tr("Remember password"));

    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->setAlignment(Qt::AlignLeft);
    passwordLayout->addWidget(new QLabel(tr("Password:")));
    passwordLayout->addWidget(m_passwordEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme("ok", QIcon(":/images/ok.png")));
    buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon::fromTheme("process-stop"));

    QVBoxLayout *masterLayout = new QVBoxLayout;
    masterLayout->setAlignment(Qt::AlignTop);
    masterLayout->setContentsMargins(20, 20, 20, 20);
    masterLayout->addLayout(descriptionLayout);
    masterLayout->addSpacerItem(new QSpacerItem(0, 20));
    masterLayout->addLayout(passwordLayout);
    masterLayout->addSpacerItem(new QSpacerItem(0, 10));
    masterLayout->addWidget(m_showPassword);
    masterLayout->addWidget(m_rememberPassword);
    masterLayout->addWidget(buttonBox);
    this->setLayout(masterLayout);

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(true);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    m_showPassword->setChecked(false);
    m_rememberPassword->setChecked(false);
    adjustEchoMode();

    connect(m_showPassword, SIGNAL(stateChanged(int)), this, SLOT(adjustEchoMode()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setWindowTitle(tr("Password required"));
}


/*! Destructor
 */
PasswordDialog::~PasswordDialog()
{

}


/*! \return the password entered by the user.
 */
QString PasswordDialog::password() const
{
    return m_passwordEdit->text();
}


/*! Set the password presented to the user in the edit field
 *
 * \param password the password to be presented
 */
void PasswordDialog::setPassword(const QString &password)
{
    m_passwordEdit->setText(password);
}


/*! Informs if the user wants the password to be stored in the configuration.
 *
 * \return true, if the password should be stored, otherwise false
 */
bool PasswordDialog::rememberPassword() const
{
    return m_rememberPassword->isChecked();
}


/*! Set, if the password should be stored in the configuration
 *
 * \param checked is true, if password should be stored, otherwise false
 */
void PasswordDialog::setRememberPassword(bool checked)
{
    m_rememberPassword->setChecked(checked);
}


/*! Toggels the echo mode of the password edit field in dependency if the password
 *  should be displayed.
 */
void PasswordDialog::adjustEchoMode()
{
    if (m_showPassword->isChecked()) {
        m_passwordEdit->setEchoMode(QLineEdit::Normal);
    } else {
        m_passwordEdit->setEchoMode(QLineEdit::Password);
    }
}
