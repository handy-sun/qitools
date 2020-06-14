INCLUDEPATH += $$PWD

PRECOMPILED_HEADER = $$PWD/stable.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/qitoolswindow.cpp \

HEADERS += \
    $$PWD/qitoolswindow.h \
    $$PWD/stable.h

FORMS += \
    $$PWD/qitoolswindow.ui \

include($$PWD/property/property.pri)
