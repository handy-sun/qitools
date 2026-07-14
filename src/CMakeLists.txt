cmake_minimum_required(VERSION 3.10)

set(PROJECT_AUTHOR "sooncheer")
project("qitools" VERSION 0.2.0 LANGUAGES CXX C)

include(CTest)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../CMake")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id)
# Normalize AppleClang to clang for consistent naming across macOS toolchains
if(compiler_id STREQUAL "appleclang")
    set(compiler_id "clang")
endif()
set(app_name "${PROJECT_NAME}-${compiler_id}")

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    set(app_name "${app_name}-d")
endif()


if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Source files are UTF-8 without BOM. Keep MSVC from interpreting them with
# the active Windows code page, matching the existing qmake configuration.
if(MSVC)
    add_compile_options(
        "$<$<COMPILE_LANGUAGE:C>:/utf-8>"
        "$<$<COMPILE_LANGUAGE:CXX>:/utf-8>"
    )
endif()

# Prefer Qt6, fall back to Qt5
find_package(Qt6 COMPONENTS Core QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 CONFIG REQUIRED Core)
    message(STATUS "Using Qt5: ${Qt5_VERSION}")
else()
    message(STATUS "Using Qt6: ${Qt6_VERSION}")
endif()

option(QITOOLS_MAC_BUNDLE "Auto deploy to qitools.app package dir" ON)

if(APPLE AND QITOOLS_MAC_BUNDLE)
    set(PLUGIN_PATH "${EXECUTABLE_OUTPUT_PATH}/${app_name}.app/Contents/MacOS/plugins")
else()
    set(PLUGIN_PATH "${EXECUTABLE_OUTPUT_PATH}/plugins")
endif()

# Prevent Visual Studio from creating Release/Debug subdirectories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_PATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PLUGIN_PATH}")

# For multi-config generators (Visual Studio), set per-config directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PLUGIN_PATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PLUGIN_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PLUGIN_PATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PLUGIN_PATH}")

add_subdirectory(codecconvert)
add_subdirectory(cppcodegen)
add_subdirectory(colortool)
add_subdirectory(simpledownload)
add_subdirectory(audio)
add_subdirectory(imageconvert)
add_subdirectory(glsltool)
add_subdirectory(core)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
