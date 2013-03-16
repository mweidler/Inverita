#-------------------------------------------------
#
# Project created by QtCreator 2.4
#
#-------------------------------------------------

QT += core gui

TARGET = ../inverita
TEMPLATE = app
MOC_DIR = ../mocs
OBJECTS_DIR = ../objs

TRANSLATIONS = \
    ../translations/inverita_de.ts

DEPENDPATH += \
    ../translations \
    ../images

RESOURCES += \
    ../resources.qrc

!isEmpty(TRANSLATIONS) {

   isEmpty(QMAKE_LRELEASE) {
     win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
     else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
   }

   isEmpty(TS_DIR):TS_DIR = ../translations

   TSQM.name = lrelease ${QMAKE_FILE_IN}
   TSQM.input = TRANSLATIONS
   TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
   TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
   TSQM.CONFIG = no_link
   QMAKE_EXTRA_COMPILERS += TSQM
   PRE_TARGETDEPS += compiler_TSQM_make_all
} else:message(No translation files in project)

#TSQM.name = lrelease ${QMAKE_FILE_IN}
#TSQM.input = TRANSLATIONS
#TSQM.output = ../translations/${QMAKE_FILE_BASE}.qm
#TSQM.commands = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN}
#TSQM.config = no_link
#QMAKE_EXTRA_COMPILERS += TSQM
#PRE_TARGETDEPS += compiler_TSQM_make_all

SOURCES += \
    main.cpp \
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
    SnapshotTableView.cpp \
    DigestsMap.cpp \
    IOBuffer.cpp

HEADERS += \
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
    SnapshotTableView.h \
    DigestsMap.h \
    IOBuffer.h

