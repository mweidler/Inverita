/*
 * BackupHistoryList.h
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

#ifndef HEADER_BACKUPLIST_INC
#define HEADER_BACKUPLIST_INC

#include <QtCore>
#include <QString>
#include <QList>
#include <QListIterator>
#include <QStringListModel>
#include <sys/stat.h>


/*****************************************************************************
 * Container for mata information and hash value.
 *****************************************************************************/
typedef struct BackupEntry {
    struct stat meta;
    QString     origin;
    int         files;
} BackupEntry;


/*****************************************************************************
 * Base type storage definition
 *****************************************************************************/
typedef QList<BackupEntry> BackupList;
typedef QListIterator<BackupEntry> BackupListIterator;


/*****************************************************************************
 * Container class for storing and handling validation information.
 *****************************************************************************/
class BackupListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    BackupListModel(QObject *parent = 0);
    virtual ~BackupListModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    void setBackupList(const BackupList &list);
    BackupList backupList();

    int Load(const QString &organization);
    void Save();
    void SaveAs(const QString &organization);

public slots:
    void onDataChanged();

protected:

private:
    BackupList m_list;
    QString    m_organization;
};

#endif // ! HEADER_BACKUPLIST_INC
