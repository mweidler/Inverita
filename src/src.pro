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

TRANSLATIONS = ../inverita_de.ts

RESOURCES += \
        ../resources.qrc

SOURCES += main.cpp\
    BackupSelectorUI.cpp \
    BackupListModel.cpp \
    Configuration.cpp \
    ConfigurationListUI.cpp \
    ControlUI.cpp \
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
    VerifyEngine.cpp \
    FilesystemInfo.cpp \
    DriveCapacityUI.cpp \
    ProgressDialog.cpp \
    ConfigurationDialog.cpp \
    Snapshot.cpp \
    SnapshotListUI.cpp \
    SnapshotListModel.cpp \
    PasswordDialog.cpp \
    Backup.cpp \
    Checksum.cpp \
    InveritaWindow.cpp \
    SnapshotTableView.cpp

HEADERS  += \
    BackupSelectorUI.h \
    BackupListModel.h \
    Configuration.h \
    ConfigurationListUI.h \
    ControlUI.h \
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
    VerifyEngine.h \
    FilesystemInfo.h \
    DriveCapacityUI.h \
    AbstractDriveCapacityModel.h \
    ProgressDialog.h \
    ConfigurationDialog.h \
    Snapshot.h \
    SnapshotListUI.h \
    SnapshotListModel.h \
    PasswordDialog.h \
    Backup.h \
    Checksum.h \
    InveritaWindow.h \
    SnapshotTableView.h

