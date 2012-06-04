/**
 * AboutDialog.cpp
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


#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "AboutDialog.h"


AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    static const QString text = tr("<b>INVERITA Personal Backup</b> Version 0.1<br>"
                                   "Copyright (c) 2012 Marc Weidler. All rights reserved.<br><br>"
                                   "'INVERITA Personal Backup' is licensed under the BSD license (simplified, 2-clause).<br>"
                                   "This software uses Qt library, which is copyright (c) 2011 Nokia Corp.<br>"
                                   "This software uses parts PolarSSL library, which is copyright (c) 2011 Polar Corp.<br>"
                                   "This software uses parts ... library, which is copyright (c) 2011 xxx Corp.<br>"
                                  );

    QLabel *labelTitle =  new QLabel(text);

    QPixmap pixmap(":/images/backup-icon.png");
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(pixmap);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(labelImage);
    hlayout->addWidget(labelTitle);

    QVBoxLayout *masterLayout = new QVBoxLayout;
    masterLayout->setContentsMargins(20, 20, 20, 20);
    masterLayout->addLayout(hlayout);
    masterLayout->addWidget(buttonBox);
    this->setLayout(masterLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    setMinimumSize(640, 300);
    resize(640, 300);
    setWindowTitle(tr("About INVERITA Personal Backup"));
}

AboutDialog::~AboutDialog()
{

}

