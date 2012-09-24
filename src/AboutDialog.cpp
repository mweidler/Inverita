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
#include <QApplication>
#include <QIcon>

#ifdef QT_ARCH_X86_64
#define INVERITA_ARCH "amd64"
#else
#define INVERITA_ARCH "i386"
#endif

#ifndef QT_LARGEFILE_SUPPORT
#error Will not compile without 'large file support'
#endif


/*! Constructs a new AboutDialog object with UI elements
 *
 * \param parent the parent ui element
 */
AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    static const QString text =
        tr("<b>INVERITA Personal Backup</b> Version %1 (%2)<br>"
           "Copyright (C) 2012 Marc Weidler (marc.weidler@web.de)<br>"
           "All rights reserved.") + "<br><br>" +
        QLatin1String(
            "INVERITA is free software: you can redistribute it and/or modify "
            "it under the terms of the GNU General Public License as published by "
            "the Free Software Foundation, either version 3 of the License, or "
            "(at your option) any later version.<br><br>"
            "INVERITA is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
            "GNU General Public License for more details: http://www.gnu.org/licenses/<br><br>"
            "INVERITA uses Qt library %3<br>"
            "Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).<br><br>"
            "INVERITA makes use of EncFS (http://www.arg0.net/encfs)<br>"
            "Copyright (C) 2003-2007, Valient Gough (vgough@pobox.com)<br><br>"
            "INVERITA contains FIPS-180-1 compliant SHA-1 implementation from PolarSSL library (http://www.polarssl.org) "
            "Copyright (C) 2006-2010, Brainspark B.V.<br><br>"
            "INVERITA contains icons from Artua (http://www.artua.com)<br>"
            "that are free for non-commercial use.<br><br>"
            "INVERITA contains icons from 'Faenza Icons' (http://tiheum.deviantart.com)<br>"
            "Copyright (C) 2010-2012, Matthieu James (~tiheum)<br><br>"
            "INVERITA contains icons from Joker Design (http://www.joker-design.com)<br>"
            "that are licensed under 'CC Attribution-Noncommercial-No Derivate 3.0'.<br>"
        );

    QLabel *labelAboutText = new QLabel(text.arg(INVERITA_COMMIT_VERSION).arg(INVERITA_ARCH).arg(qVersion()));
    labelAboutText->setWordWrap(true);

    QLabel *labelAppImage = new QLabel;
    labelAppImage->setPixmap(QApplication::windowIcon().pixmap(96, 96));

    QLabel *labelGPLImage = new QLabel;
    labelGPLImage->setPixmap(QPixmap(":/images/gplv3.png"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);

    QVBoxLayout *imagesLayout = new QVBoxLayout;
    imagesLayout->setAlignment(Qt::AlignTop);
    imagesLayout->addWidget(labelAppImage);
    imagesLayout->addSpacerItem(new QSpacerItem(0, 40));
    imagesLayout->addWidget(labelGPLImage);

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->setAlignment(Qt::AlignLeft);
    contentLayout->addLayout(imagesLayout);
    contentLayout->addSpacerItem(new QSpacerItem(20, 0));
    contentLayout->addWidget(labelAboutText, 1, Qt::AlignTop);

    QVBoxLayout *masterLayout = new QVBoxLayout;
    masterLayout->setContentsMargins(20, 20, 20, 20);
    masterLayout->addLayout(contentLayout);
    masterLayout->addWidget(buttonBox);
    this->setLayout(masterLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    setWindowTitle(tr("About INVERITA Personal Backup"));
    setMinimumSize(700, 500);
    resize(700, 600);
}


/*! Destructor
 */
AboutDialog::~AboutDialog()
{
}

