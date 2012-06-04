/*
 * BackupHistoryUI.hpp
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

#ifndef HEADER_BACKUPHISTORYUI_INC
#define HEADER_BACKUPHISTORYUI_INC

#include <QtGui>

class BackupHistoryUI : public QFrame
{
    Q_OBJECT

public:
    BackupHistoryUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~BackupHistoryUI();

    int currentSelection();

signals:
    void validateBackup();
    void deleteBackup();
    void reload();

public slots:
    void OnItemSelected();
    void OnDelete();

protected:
    QTableView  *m_tableView;
    QPushButton *m_buttonValidate;
    QPushButton *m_buttonDelete;
    QPushButton *m_buttonReload;
};


class BackupHistoryUIDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    BackupHistoryUIDelegate(QWidget *parent = 0);
    ~BackupHistoryUIDelegate();

    QString displayText(const QVariant &value, const QLocale &locale) const;

protected:
};

#endif
