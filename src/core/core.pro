include(../../qitools.pri)
TEMPLATE = app

QT *= core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = $$OUT_PATH

TARGET = $$APP_TARGET

RES_PATH = $$OUT_PATH/../resource

CONFIG += c++11 \
    no_keywords \
    skip_target_version_ext

DEFINES += QT_MESSAGELOGCONTEXT \
    QT_DEPRECATED_WARNINGS \
#    USE_QAUDIODECODER

msvc: lessThan(QMAKE_MSC_VER, 1900): DEFINES *= Q_COMPILER_INITIALIZER_LISTS

#linux: !android: gcc: QMAKE_LFLAGS += -no-pie

win32 {
    CONFIG += precompiled_header
    PRECOMPILED_HEADER = $$PWD/stable.h
    SUFFIX = .dll
} else: osx {
    SUFFIX = .dylib
} else: unix {
    SUFFIX = .so
}

DEFINES += DYNAMIC_SUFFIX=\\\"$${SUFFIX}\\\"

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/pluginmanager.cpp \
    $$PWD/qitoolswindow.cpp \

HEADERS += \
    $$PWD/pluginmanager.h \
    $$PWD/pluginterface.h \
    $$PWD/qitoolswindow.h \
    $$PWD/stable.h

RESOURCES += $$RES_PATH/qitools.qrc

RC_ICONS = $$RES_PATH/toolsimage.ico

ICON = $$RES_PATH/toolsimage.icns

VERSION = 0.12.1
DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"

TRANSLATIONS = core-zh_CN.ts
