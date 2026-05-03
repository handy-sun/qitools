cmake_minimum_required(VERSION 3.10)

set(PROJECT_AUTHOR "sooncheer")
project("qitools" VERSION 0.1.15 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../CMake")
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

# Prefer Qt6, fall back to Qt5
find_package(Qt6 COMPONENTS Core QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 CONFIG REQUIRED Core)
    message(STATUS "Using Qt5: ${Qt5_VERSION}")
else()
    message(STATUS "Using Qt6: ${Qt6_VERSION}")
endif()

option(QITOOLS_MAC_BUNDLE "Auto deploy to qitools.app package dir" ON)

if(QITOOLS_MAC_BUNDLE)
    set(PLUGIN_PATH "${EXECUTABLE_OUTPUT_PATH}/${app_name}.app/Contents/MacOS/plugins")
else()
    set(PLUGIN_PATH "${EXECUTABLE_OUTPUT_PATH}/plugins")
endif()

add_subdirectory(codecconvert)
add_subdirectory(cppcodegen)
add_subdirectory(colortool)
add_subdirectory(simpledownload)
add_subdirectory(audio)
add_subdirectory(imageconvert)
add_subdirectory(glsltool)
add_subdirectory(core)
