/*
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


#include "AboutDialog.h"
#include "version.h"

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>


/*! Constructs a new AboutDialog object with UI elements
 *
 * \param parent the parent ui element
 */
AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    static const QString text =
        tr(
            "<b>INVERITA Personal Backup</b> Version %1<br>"
            "Copyright (C) 2012 Marc Weidler (marc.weidler@web.de)<br>"
            "All rights reserved.<br><br>"
            "INVERITA is free software: you can redistribute it and/or modify<br>"
            "it under the terms of the GNU General Public License as published by<br>"
            "the Free Software Foundation, either version 3 of the License, or<br>"
            "(at your option) any later version.<br><br>"
            "INVERITA is distributed in the hope that it will be useful,<br>"
            "but WITHOUT ANY WARRANTY; without even the implied warranty of<br>"
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br>"
            "GNU General Public License for more details.<br>"
            "http://www.gnu.org/licenses/<br><br>"
            "INVERITA uses Qt library %2<br>"
            "Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).<br><br>"
            "INVERITA uses parts of PolarSSL library (http://www.polarssl.org)<br>"
            "FIPS-180-1 compliant SHA-1 implementation<br>"
            "Copyright (C) 2006-2010, Brainspark B.V.<br><br>"
            "INVERITA uses icons from Artua (http://www.artua.com)<br>"
            "that are free for non-commercial use.<br><br>"
            "INVERITA uses icons from Joker Design (http://www.joker-design.com)<br>"
            "that are licensed under 'CC Attribution-Noncommercial-No Derivate 3.0'.<br>"
        );

    QLabel *labelAboutText = new QLabel(text.arg(INVERITA_COMMIT_VERSION).arg(qVersion()));

    QPixmap pixmap(":/images/backup-icon.png");
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(pixmap);

    QPixmap gplpixmap(":/images/gplv3.png");
    QLabel *labelGpl = new QLabel;
    labelGpl->setPixmap(gplpixmap);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setAlignment(Qt::AlignTop);
    vlayout->addWidget(labelImage);
    vlayout->addWidget(labelGpl);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setAlignment(Qt::AlignLeft);
    hlayout->addLayout(vlayout);
    hlayout->addWidget(labelAboutText);

    QVBoxLayout *masterLayout = new QVBoxLayout;
    masterLayout->setContentsMargins(20, 20, 20, 20);
    masterLayout->addLayout(hlayout);
    masterLayout->addWidget(buttonBox);
    this->setLayout(masterLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    setMinimumSize(700, 550);
    resize(700, 550);
    setWindowTitle(tr("About INVERITA Personal Backup"));
}


/*! Destructor
 */
AboutDialog::~AboutDialog()
{
}

