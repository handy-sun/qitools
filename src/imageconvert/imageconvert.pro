include(../dynamicplugin.pri)

TARGET = imageconvert

FORMS += \
    $$PWD/imagetoolwidget.ui

HEADERS += \
    $$PWD/imagetoolwidget.h

SOURCES += \
    $$PWD/imagetoolwidget.cpp

TRANSLATIONS = imageconvert-zh_CN.ts
