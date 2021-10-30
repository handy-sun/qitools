include(../dynamicplugin.pri)

QT *= network

TARGET = simpledownload

HEADERS += \
    $$PWD/networkcontrol.h \
    $$PWD/downloadwidget.h

SOURCES += \
    $$PWD/networkcontrol.cpp \
    $$PWD/downloadwidget.cpp

FORMS += \
    $$PWD/downloadwidget.ui
