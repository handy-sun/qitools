include(../dynamicplugin.pri)

QT *= multimedia

include(minimp3/minimp3.pri)

DEFINES *= SLIDERDOWN_WHEN_DRAGGING

FORMS += \
    $$PWD/audiowidget.ui

HEADERS += \
    $$PWD/audiodataplay.h \
    $$PWD/audiowidget.h \
    $$PWD/struct_id3v2.h \
    $$PWD/wavfile.h

SOURCES += \
    $$PWD/audiodataplay.cpp \
    $$PWD/audiowidget.cpp \
    $$PWD/wavfile.cpp


