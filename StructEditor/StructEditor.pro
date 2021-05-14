QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Dialogs/AliasDialog.cpp \
    Dialogs/EnumDialog.cpp \
    Dialogs/StructDialog.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Dialogs/AliasDialog.h \
    Dialogs/EnumDialog.h \
    Dialogs/StructDialog.h \
    MainWindow.h \

FORMS += \
    Dialogs/AliasDialog.ui \
    Dialogs/EnumDialog.ui \
    Dialogs/StructDialog.ui \
    MainWindow.ui

TRANSLATIONS += \
    StructEditor_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StructReader/release/ -lStructReader
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StructReader/debug/ -lStructReader
else:unix: LIBS += -L$$OUT_PWD/../StructReader/ -lStructReader

INCLUDEPATH += $$PWD/../StructReader
DEPENDPATH += $$PWD/../StructReader
