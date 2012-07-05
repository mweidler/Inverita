/*
 * PasswordDialog.cpp
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


#include "PasswordDialog.h"

#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

PasswordDialog::PasswordDialog(QWidget *parent) : QDialog(parent)
{
    m_passwordEdit = new QLineEdit();
    QLabel *verifyText = new QLabel(tr("The contents of this backup is encrypted with a individual password.\n"
                                       "Please enter the password to access this backup.")
                                    );

    QPixmap pixmap(":/images/backup-icon.png");
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(pixmap);


    QHBoxLayout *descriptionLayout = new QHBoxLayout;
    descriptionLayout->setAlignment(Qt::AlignLeft);
    descriptionLayout->addWidget(labelImage);
    descriptionLayout->addWidget(verifyText);

    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->setAlignment(Qt::AlignLeft);
    passwordLayout->addWidget(new QLabel(tr("Password:")));
    passwordLayout->addWidget(m_passwordEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme("ok"));
    buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon::fromTheme("stop"));

    QVBoxLayout *verifyLayout = new QVBoxLayout;
    verifyLayout->setAlignment(Qt::AlignTop);
    m_showPassword = new QCheckBox(tr("Show password"));
    m_rememberPassword = new QCheckBox(tr("Remember password"));
    verifyLayout->addLayout(descriptionLayout);
    verifyLayout->addLayout(passwordLayout);
    verifyLayout->addWidget(m_showPassword);
    verifyLayout->addWidget(m_rememberPassword);
    verifyLayout->addWidget(buttonBox);
    this->setLayout(verifyLayout);

    m_showPassword->setChecked(true);
    m_rememberPassword->setChecked(false);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setWindowTitle(tr("Password required"));
    setMinimumSize(300, 200);
    //resize(620, 480);
}

PasswordDialog::~PasswordDialog()
{

}

QString PasswordDialog::password()
{
    return m_passwordEdit->text();
}

void PasswordDialog::setPassword(const QString &password)
{
    m_passwordEdit->setText(password);
}

bool PasswordDialog::rememberPassword()
{
    return m_rememberPassword->isChecked();
}
