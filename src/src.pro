#-------------------------------------------------
#
# Project created by QtCreator 2012-04-02T17:16:26
#
#-------------------------------------------------

QT       += core gui

TARGET = ../Inverita
TEMPLATE = app
MOC_DIR = ../mocs
OBJECTS_DIR = ../objs

RESOURCES += \
        ../resources.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    BackupSelectorUI.cpp \
    BackupHistoryUI.cpp \
    BackupHistoryList.cpp \
    BackupListModel.cpp \
    ConfigurationUI.cpp \
    Configuration.cpp \
    ConfigurationListUI.cpp \
    ControlUI.cpp \
    ProgressDialogUI.cpp \
    Utilities.cpp \
    BackupEngine.cpp \
    EraseEngine.cpp \
    Traverser.cpp \
    CopyTraverser.cpp \
    ScanTraverser.cpp \
    EraseTraverser.cpp \
    SnapshotMetaInfo.cpp \
    AboutDialog.cpp \
    sha1.c \
    SignatureMap.cpp \
    ApplicationException.cpp \
    WorkerEngine.cpp \
    ValidateTraverser.cpp \
    ValidateEngine.cpp \
    VerifyEngine.cpp

HEADERS  += mainwindow.h \
    BackupSelectorUI.h \
    BackupHistoryUI.h \
    BackupHistoryList.h \
    BackupListModel.h \
    ConfigurationUI.h \
    Configuration.h \
    ConfigurationListUI.h \
    ControlUI.h \
    ProgressDialogUI.h \
    Utilities.h \
    BackupEngine.h \
    EraseEngine.h \
    Traverser.h \
    CopyTraverser.h \
    ScanTraverser.h \
    EraseTraverser.h \
    SnapshotMetaInfo.h \
    AboutDialog.h \
    sha1.h \
    SignatureMap.h \
    ApplicationException.h \
    WorkerEngine.h \
    ValidateTraverser.h \
    ValidateEngine.h \
    VerifyEngine.h

