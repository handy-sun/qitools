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


