TEMPLATE  = subdirs
CONFIG   += ordered

# MacOS
# QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
CONFIG += sdk_no_version_check

SUBDIRS  += \
    cppcodegen \
    colortool \
    simpledownload \
    audio \
    imageconvert \
    glsltool \
    codecconvert \
    core \
