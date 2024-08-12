include(../dynamicplugin.pri)

TARGET = codegen

SOURCES += \
    $$PWD/propertygenerator.cpp \
    $$PWD/propertymanager.cpp

HEADERS += \
    $$PWD/propertygenerator.h \
    $$PWD/propertymanager.h \

FORMS += \
    $$PWD/propertygenerator.ui

TRANSLATIONS = codegen-zh_CN.ts
