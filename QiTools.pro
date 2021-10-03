
QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix: QT += x11extras

TEMPLATE = app

DESTDIR = $$PWD/bin

include(third_party/third_party.pri)
include(src/src.pro)

RESOURCES += \
    $$PWD/resource/qitools.qrc

RC_ICONS = $$PWD/resource/toolsimage.ico

VERSION = 0.1.2
