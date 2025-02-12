include(../dynamicplugin.pri)

TARGET = codecconvert

CED_ROOT = $$PWD/ced

CED_SOURCES += \
    $${CED_ROOT}/compact_enc_det/compact_enc_det.cc \
    $${CED_ROOT}/compact_enc_det/compact_enc_det_hint_code.cc \
    $${CED_ROOT}/util/encodings/encodings.cc \
    $${CED_ROOT}/util/languages/languages.cc

INCLUDEPATH += $${CED_ROOT}

FORMS += \
    $$PWD/codecconvertwidget.ui

HEADERS += \
    $$PWD/codecconvertwidget.h

SOURCES += \
    $$PWD/codecconvertwidget.cpp \
    $${CED_SOURCES}

TRANSLATIONS = codecconvert-zh_CN.ts
