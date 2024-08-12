include(../dynamicplugin.pri)

TARGET = glsltool

SOURCES += \
    $$PWD/glslcodeconvert.cpp

HEADERS += \
    $$PWD/glslcodeconvert.h

FORMS += \
    $$PWD/glslcodeconvert.ui

TRANSLATIONS = glsltool-zh_CN.ts
