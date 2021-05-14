QT -= gui

TEMPLATE = lib
DEFINES += STRUCTREADER_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataReader.cpp \
    Entity/Alias.cpp \
    Entity/Enum.cpp \
    Entity/IBaseEntity.cpp \
    Entity/Struct.cpp

HEADERS += \
    DataReader.h \
    Entity/Alias.h \
    Entity/Enum.h \
    Entity/IBaseEntity.h \
    Entity/Struct.h \
    StructReader_global.h \
    common/defines.h \
    common/macros.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
