TEMPLATE = app

QT *= core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = $$PWD/../../bin

contains(QT_ARCH, i386): ARCHITECTURE = x86
else: contains(QT_ARCH, x86_64): ARCHITECTURE = amd64
else: ARCHITECTURE = $QT_ARCH

TARGET_NAME = qitools-$${ARCHITECTURE}-$${QMAKE_CC}

CONFIG(debug, debug | release): TARGET_NAME = $${TARGET_NAME}-d

TARGET = $$TARGET_NAME

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
} else: unix {
    SUFFIX = .so
} else: mac {
    SUFFIX = .dylib
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

RESOURCES += $$PWD/../../resource/qitools.qrc

RC_ICONS = $$PWD/../../resource/toolsimage.ico

VERSION = 0.9.1
DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"
