INCLUDEPATH += $$PWD

include($$PWD/minimp3/minimp3.pri)

contains(CONFIG, "static") {
    build_pass: message("static libs")
}
