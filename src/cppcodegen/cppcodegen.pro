include(../dynamicplugin.pri)

TARGET = cppcodegen

SOURCES += \
    $$PWD/propertygenerator.cpp \
    $$PWD/propertymanager.cpp

HEADERS += \
    $$PWD/propertygenerator.h \
    $$PWD/propertymanager.h \

FORMS += \
    $$PWD/propertygenerator.ui

TRANSLATIONS = cppcodegen-zh_CN.ts
