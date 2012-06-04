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

#ifndef HEADER_BACKUPHISTORYLIST_INC
#define HEADER_BACKUPHISTORYLIST_INC

#include <QtCore>
#include <QString>
#include <QList>
#include <QListIterator>
#include <QFileInfo>
#include <QAbstractTableModel>
#include <sys/stat.h>


/******************************************************************************
 * Container for mata information
 ******************************************************************************/
typedef struct BackupHistoryEntry {
    QDateTime   execution;
    QString     origin;
    int         files;
    int         totalSize;
    QString     location;
    QString     name;
} BackupHistoryEntry;


/*******************************************************************************
 * Base type storage definition
 ******************************************************************************/
typedef QList<BackupHistoryEntry> BaseBackupHistoryList;
typedef QListIterator<BackupHistoryEntry> BackupHistoryListIterator;


/*******************************************************************************
 * Model class for storing and handling backup history.
 ******************************************************************************/
class BackupHistoryList : public BaseBackupHistoryList, public QAbstractTableModel
{
public:

    /** Constructor */
    BackupHistoryList(QObject *parent = 0);

    /** model<->view handling */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /** Misc */
    void investigate(QString &origin);

    /** Destructor */
    virtual ~BackupHistoryList();

protected:

private:
    QStringList m_headerLabels;
};

#endif // ! HEADER_BACKUPHISTORYLIST_INC
