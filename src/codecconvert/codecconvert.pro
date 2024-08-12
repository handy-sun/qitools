include(../dynamicplugin.pri)

TARGET = codecconvert

FORMS += \
    $$PWD/codecconvertwidget.ui

HEADERS += \
    $$PWD/codecconvertwidget.h

SOURCES += \
    $$PWD/codecconvertwidget.cpp

LIBS += -L$$PWD -luchardet
TRANSLATIONS = codecconvert-zh_CN.ts
