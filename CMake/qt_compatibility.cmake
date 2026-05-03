# Qt5/Qt6 compatibility layer
# Include this in submodules instead of calling find_package directly.
#
# Usage:
#   include(qt_compatibility)
#   target_link_libraries(${PROJECT_NAME} ${QT_LIBS_PREFIX}::Widgets ${QT_LIBS_PREFIX}::Core)
#
# Provides:
#   QT_LIBS_PREFIX  - "Qt5" or "Qt6"
#   QT_VERSION_MAJOR - 5 or 6
#   qt_add_resources()  - wraps qt5_add_resources / qt6_add_resources
#   qt_add_translation() - wraps qt5_add_translation / qt6_add_translation

include(CMakeParseArguments)

find_package(Qt6 COMPONENTS Core QUIET)
if(Qt6_FOUND)
    set(QT_LIBS_PREFIX "Qt6")
    set(QT_VERSION_MAJOR 6)
    message(STATUS "Qt6 found: ${Qt6_VERSION}")
else()
    find_package(Qt5 5.15 CONFIG REQUIRED Core)
    set(QT_LIBS_PREFIX "Qt5")
    set(QT_VERSION_MAJOR 5)
    message(STATUS "Qt5 found: ${Qt5_VERSION}")
endif()

# Wrapper: qt_add_resources(OUTPUT_VAR qrc_file ...)
function(qt_add_resources OUT_VAR)
    if(QT_VERSION_MAJOR EQUAL 6)
        qt6_add_resources(${OUT_VAR} ${ARGN})
    else()
        qt5_add_resources(${OUT_VAR} ${ARGN})
    endif()
    set(${OUT_VAR} "${${OUT_VAR}}" PARENT_SCOPE)
endfunction()

# Wrapper: qt_add_translation(OUT_VAR ts_files ...)
function(qt_add_translation OUT_VAR)
    if(QT_VERSION_MAJOR EQUAL 6)
        qt6_add_translation(${OUT_VAR} ${ARGN})
    else()
        qt5_add_translation(${OUT_VAR} ${ARGN})
    endif()
    set(${OUT_VAR} "${${OUT_VAR}}" PARENT_SCOPE)
endfunction()
