INCLUDEPATH += $$PWD

greaterThan(QT_MAJOR_VERSION, 4) {
    TARGET_ARCH = $${QT_ARCH}
} else {
    TARGET_ARCH = $${QMAKE_HOST.arch}
}

TARGET_NAME = QiTools_$${TARGET_ARCH}_$${QMAKE_CC}

CONFIG(debug, debug | release) {
    TARGET_NAME = $${TARGET_NAME}_d
}

TARGET = $$TARGET_NAME

CONFIG += c++11 \
    no_keywords \
    skip_target_version_ext

DEFINES += QT_MESSAGELOGCONTEXT \
    QT_DEPRECATED_WARNINGS \
#    Q_AUDIO_DECODER


msvc: lessThan(QMAKE_MSC_VER, 1900): DEFINES += Q_COMPILER_INITIALIZER_LISTS

win32 {
    CONFIG += precompiled_header
    PRECOMPILED_HEADER = $$PWD/stable.h
}

linux: !android: gcc: QMAKE_LFLAGS += -no-pie
#LIBS += -lxcb -lX11

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/qitoolswindow.cpp \

HEADERS += \
    $$PWD/qitoolswindow.h \
    $$PWD/stable.h

FORMS += \
    $$PWD/qitoolswindow.ui \


include($$PWD/property/property.pri)
include($$PWD/color/color.pri)
include($$PWD/glsltool/glsltool.pri)
include($$PWD/vsc_vsix/vsc_vsix.pri)
include($$PWD/imageconvert/imageconvert.pri)
include($$PWD/codecconvert/codecconvert.pri)
include($$PWD/audio/audio.pri)
