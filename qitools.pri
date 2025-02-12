!isEmpty(QITOOLS_PRI_INCLUDED):error("qitools.pri already included")
QITOOLS_PRI_INCLUDED = 1

contains(QT_ARCH, i386): ARCHITECTURE = x86
else: contains(QT_ARCH, x86_64): ARCHITECTURE = amd64
else: ARCHITECTURE = $${QT_ARCH}

TARGET_NAME = qitools-$${ARCHITECTURE}-$$basename(QMAKE_CC)

CONFIG(debug, debug | release): TARGET_NAME = $${TARGET_NAME}-d

OUT_PATH = $$PWD/bin
APP_TARGET = "$$TARGET_NAME"

osx {

    # check if IDE_BUILD_TREE is actually an existing Qt Creator.app,
    # for building against a binary package
#    exists($$OUT_PATH/$${APP_TARGET}.app/Contents/MacOS/$${APP_TARGET}): APP_BUNDLE = $$IDE_BUILD_TREE
#    else: APP_BUNDLE = $$OUT_PATH/$${APP_TARGET}.app
#    isEmpty(IDE_OUTPUT_PATH): IDE_OUTPUT_PATH = $$APP_BUNDLE/Contents

    PLUGIN_PATH = $$OUT_PATH/$${APP_TARGET}.app/Contents/MacOS/plugins
} else {
    PLUGIN_PATH = $$OUT_PATH/plugins
}

## msvc compile error if a file enconding is utf-8 without BOM
win32-msvc*: {
    QMAKE_CFLAGS *= /utf-8
    QMAKE_CXXFLAGS *= /utf-8
}
