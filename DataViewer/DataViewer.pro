QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataViewWidget/ContextMenuController.cpp \
    DataViewWidget/Cursor/IOverlayWidget.cpp \
    DataViewWidget/Cursor/CursorController.cpp \
    DataViewWidget/Cursor/IBeamCursorOverlay.cpp \
    Entity/BackupInfo.cpp \
    StructViewWidget/StructSelectDialog.cpp \
    StructViewWidget/StructViewWidget.cpp \
    Utils/ByteArrayReader.cpp \
    Utils/RestoreThread.cpp \
    Utils/SaveBackupFileThread.cpp \
    Utils/Utils.cpp \
    common/FileReader.cpp \
    MainWindow/FileViewWidget.cpp \
    MainWindow/MainWindow.cpp \
    common/RenderColor.cpp \
    DataViewWidget/DataRenderWidget.cpp \
    common/Settings.cpp \
    main.cpp

HEADERS += \
    DataViewWidget/ContextMenuController.h \
    DataViewWidget/Cursor/CursorController.h \
    DataViewWidget/Cursor/IBeamCursorOverlay.h \
    DataViewWidget/Cursor/IOverlayWidget.h \
    Entity/BackupInfo.h \
    StructViewWidget/StructSelectDialog.h \
    StructViewWidget/StructViewWidget.h \
    Utils/ByteArrayReader.h \
    Utils/RestoreThread.h \
    Utils/SaveBackupFileThread.h \
    Utils/Utils.h \
    common/Exceptions.h \
    common/FileReader.h \
    MainWindow/FileViewWidget.h \
    MainWindow/MainWindow.h \
    common/RenderColor.h \
    DataViewWidget/DataRenderWidget.h \
    common/Settings.h \
    common/pch.h

FORMS += \
    MainWindow/FileViewWidget.ui \
    MainWindow/MainWindow.ui \
    StructViewWidget/StructSelectDialog.ui \
    StructViewWidget/StructViewWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StructReader/release/ -lStructReader
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StructReader/debug/ -lStructReader
else:unix: LIBS += -L$$OUT_PWD/../StructReader/ -lStructReader

INCLUDEPATH += $$PWD/../StructReader
DEPENDPATH += $$PWD/../StructReader
