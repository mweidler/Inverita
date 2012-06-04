/*
 * CopyTraverser.h
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

#ifndef HEADER_COPYTRAVERSER_INC
#define HEADER_COPYTRAVERSER_INC

#include "Traverser.h"
#include "SignatureMap.h"


/*! Copy file system items (file, link, dir, other) from one place to
 *  an other while using hard links for alreadz existing files on the other place.
 */
class CopyTraverser : public Traverser
{
    Q_OBJECT

public:
    CopyTraverser();
    SignatureMap &currentSignatures();
    SignatureMap &previousSignatures();
    void setCurrentBackupPath(QString &path);
    void setPreviousBackupPath(QString &path);

public:
    virtual void onFile(const QString &absoluteFilePath);
    virtual void onEnterDir(const QString &absoluteFilePath);
    virtual void onLeaveDir(const QString &absoluteFilePath);
    virtual void onLink(const QString &absoluteFilePath, const QString &linkName);
    virtual void onOther(const QString &absoluteFilePath);

protected:
    bool equalsFile(QString &newfile, QString &reference);
    bool copyFile(QString &sourcefilename, QString &targetfilename, QByteArray &hash);

protected:
    QString      m_currentBackupPath;   //!< absolute path to the current backup
    QString      m_previousBackupPath;  //!< absolute path to the previous backup
    char         m_copyBuffer[4096];    //!< buffer for file copying
    SignatureMap m_currentSignatures;   //!< hash signatures of all files in current backup
    SignatureMap m_previousSignatures;  //!< hash signatures of all files in previous backup
};

#endif
