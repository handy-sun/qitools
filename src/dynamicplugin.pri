include(../qitools.pri)

TEMPLATE      = lib
QT           *= core gui
QT           += widgets
CONFIG       += plugin \
                skip_target_version_ext
DESTDIR       = $${PLUGIN_PATH}
INCLUDEPATH  += $$PWD/core

VERSION = 1.0.0

QMAKE_TARGET_DESCRIPTION = "qitools plugin"

!CONFIG(release, debug | release) {
    DESTDIR = $${DESTDIR}/debug
}
