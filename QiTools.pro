
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

win32 {
    contains(DEFINES, WIN64) {
        DESTDIR = $$PWD/bin64        
    } else {
        DESTDIR = $$PWD/bin32
    }
}

CONFIG(debug, debug|release){
    TARGET = QiTools_d
} else {
    TARGET = QiTools
}

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

include(src/src.pri)


