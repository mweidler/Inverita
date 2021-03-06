/*
 * ProgressDialog.h
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

#ifndef HEADER_PROGRESSDIALOG_INC
#define HEADER_PROGRESSDIALOG_INC

#include "WorkerEngine.h"

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QList>
#include <QDateTime>


/*! Dialog class presenting progress information to the user
 */
class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogType { ShowTextBox, NoTextBox };
    enum DialogAbortable { NotAbortable, Abortable };

    ProgressDialog(WorkerEngine *model, DialogType type = NoTextBox, DialogAbortable abortable = Abortable, QWidget *parent = 0);
    ~ProgressDialog();

public slots:
    void update();
    void finalize();
    void display(QString message);

signals:
    void aborted();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *event);

private:
    WorkerEngine        *m_model;
    QProgressBar        *m_progressBar;
    QLabel              *m_labelRemaining;
    QTimer              *m_timer;
    QTextEdit           *m_textArea;
    QDialogButtonBox    *m_buttonBox;

    QList<WorkerStatus> m_statusHistory;
    QList<QLabel *>     m_labelList;
    int                 m_previousCurrentTask;
    bool                m_finalized;
};

#endif
