include(../dynamicplugin.pri)

TARGET = colortool

HEADERS += \
    $$PWD/colorconvert.h \
    $$PWD/screencolorpicker.h

SOURCES += \
    $$PWD/colorconvert.cpp \
    $$PWD/screencolorpicker.cpp

FORMS += \
    $$PWD/colorconvert.ui

RESOURCES +=  $$PWD/colortool.qrc

TRANSLATIONS = colortool-zh_CN.ts
