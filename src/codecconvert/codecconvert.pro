include(../dynamicplugin.pri)

TARGET = codecconvert

FORMS += \
    $$PWD/codecconvertwidget.ui

HEADERS += \
    $$PWD/codecconvertwidget.h

SOURCES += \
    $$PWD/codecconvertwidget.cpp

TRANSLATIONS = codecconvert-zh_CN.ts
