TEMPLATE      = lib
QT           += widgets
CONFIG       += plugin
DESTDIR       = $$PWD/../bin/plugins
INCLUDEPATH  += $$PWD/core

!CONFIG(release, debug | release) {
    DESTDIR = $${DESTDIR}/debug
}

