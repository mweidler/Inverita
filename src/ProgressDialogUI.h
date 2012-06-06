/**
 * ProgressDialogUI.h
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

#ifndef HEADER_PROGRESSDIALOGUI_INC
#define HEADER_PROGRESSDIALOGUI_INC

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QTextEdit>
#include <QDialogButtonBox>

#include "WorkerEngine.h"

class ProgressDialogUI : public QDialog
{
    Q_OBJECT

public:
    enum DialogType { ShowTextBox, NoTextBox };

    ProgressDialogUI(WorkerEngine *model, DialogType type = NoTextBox, QWidget *parent = 0);
    ~ProgressDialogUI();

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
    WorkerEngine     *m_model;
    QList<QLabel *>  m_labelList;
    QProgressBar     *m_progressBar;
    QLabel           *m_labelRemaining;
    int              m_previousCurrentTask;
    QTimer           *m_timer;
    QTextEdit        *m_textArea;
    QDialogButtonBox *m_buttonBox;
};

#endif
