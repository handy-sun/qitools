INCLUDEPATH += $$PWD


TARGET_NAME = QiTools

greaterThan(QT_MAJOR_VERSION, 4) {
    TARGET_ARCH = $${QT_ARCH}
} else {
    TARGET_ARCH = $${QMAKE_HOST.arch}
}

win32 {
    msvc: TARGET_NAME = $${TARGET_NAME}_msvc
    else: mingw: TARGET_NAME = $${TARGET_NAME}_mingw
} else: unix {
    gcc: TARGET_NAME = $${TARGET_NAME}_gcc
}

contains(TARGET_ARCH, x86_64) {
    TARGET_NAME = $${TARGET_NAME}64
} else {
    TARGET_NAME = $${TARGET_NAME}32
}

CONFIG(debug, debug | release) {
    TARGET_NAME = $${TARGET_NAME}_D
}

TARGET = $$TARGET_NAME

CONFIG += c++11 \
    no_keywords \
#    console

DEFINES += QT_DEPRECATED_WARNINGS \
#        qtout=qDebug()

msvc: lessThan(QMAKE_MSC_VER, 1900): DEFINES += Q_COMPILER_INITIALIZER_LISTS

win32 {
    CONFIG += precompiled_header
    PRECOMPILED_HEADER = $$PWD/stable.h
}

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

