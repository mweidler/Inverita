/*
 * AboutDialog.cpp
 *
 * This file is part of INVERITA.
 *
 * INVERITA Personal Backup Software
 * Copyright (C) 2012-2013  Marc Weidler <marc.weidler@web.de>,
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


#include "AboutDialog.h"
#include "version.h"

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QApplication>
#include <QIcon>


/*! Constructs a new AboutDialog object with UI elements
 *
 * \param parent the parent ui element
 */
AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    static const QString text[] = {
        tr("<b>INVERITA Personal Backup</b> Version %1 (%2-bit)") + "<br>" +
        QLatin1String("Copyright &copy; 2012-2013 Marc Weidler (marc.weidler@web.de). ") +
        tr("All rights reserved.") + "<br><br>" +
        QLatin1String(
            "INVERITA is free software: you can redistribute it and/or modify "
            "it under the terms of the GNU General Public License as published by "
            "the Free Software Foundation, either version 3 of the License, or "
            "(at your option) any later version.") + "<br><br>" +
        QLatin1String(
            "INVERITA is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
            "GNU General Public License for more details: http://www.gnu.org/licenses/") + "<br><br>" +
        QLatin1String(
            "INVERITA uses Qt library %3<br>"
            "Copyright &copy; 2012 Nokia Corporation and/or its subsidiary(-ies)."),
        QLatin1String(
            "INVERITA makes use of EncFS (http://www.arg0.net/encfs)<br>"
            "Copyright &copy; 2003-2007, Valient Gough (vgough@pobox.com)"),
        QLatin1String(
            "INVERITA contains FIPS-180-1 compliant SHA-1 implementation from PolarSSL library (http://www.polarssl.org) "
            "Copyright &copy; 2006-2010, Brainspark B.V."),
        QLatin1String(
            "INVERITA contains icons from 'Mac Icons'<br>"
            "Copyright &copy; 2002-2011, Artua Visual Design Agency (http://www.artua.com)"),
        QLatin1String(
            "INVERITA contains icons from 'Faenza Icons' (http://tiheum.deviantart.com)<br>"
            "Copyright &copy; 2010-2012, Matthieu James (~tiheum)"),
        QLatin1String(
            "INVERITA contains icons from 'Android Icons'<br>"
            "Copyright &copy; 2011, JOKER DESIGN (http://www.joker-design.com)")
    };

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

    QVBoxLayout *textLayout = new QVBoxLayout;
    QLabel *labelText;
    for (int i = 0; i < 6; i++) {
        if (i == 0) {
            labelText = new QLabel(text[i].arg(INVERITA_COMMIT_VERSION).arg(QSysInfo::WordSize).arg(qVersion()));
        } else {
            labelText = new QLabel(text[i]);
        }
        labelText->setTextFormat(Qt::RichText);
        labelText->setWordWrap(true);
        textLayout->addWidget(labelText);
    }

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->addLayout(imagesLayout);
    contentLayout->addSpacerItem(new QSpacerItem(20, 0));
    contentLayout->addLayout(textLayout, 1);

    QVBoxLayout *masterLayout = new QVBoxLayout;
    masterLayout->setContentsMargins(20, 20, 20, 20);
    masterLayout->addLayout(contentLayout, 1);
    masterLayout->addWidget(buttonBox);
    this->setLayout(masterLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    setWindowTitle(tr("About INVERITA Personal Backup"));
    setMinimumWidth(720);
    adjustSize();
    //setMinimumSize(720, 540);
    setMinimumSize(size());
}


/*! Destructor
 */
AboutDialog::~AboutDialog()
{
}

