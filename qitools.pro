include(qitools.pri)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS += src

osx {
    first.commands += test -d "$${PLUGIN_PATH}/../translations" || cp -r "$$PWD/bin/translations" "$${PLUGIN_PATH}/../"
    QMAKE_EXTRA_TARGETS += first
} 
