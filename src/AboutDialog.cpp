/*
 * AboutDialog.cpp
 *
 * Copyright (c) 2012, Marc Weidler <marc.weidler@web.de>
 * Ulrichstr. 12/1, 71672 Marbach, Germany
 * All rights reserved.
 *
 * 'ALVARA Personal Backup' is licensed under the BSD license (simplified, 2-clause).
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
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

