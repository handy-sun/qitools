cmake_minimum_required(VERSION 3.10)

set(PROJECT_AUTHOR "sooncheer")
project("qitools" VERSION 0.15.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../CMake")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id)
set(app_name "${PROJECT_NAME}-${compiler_id}")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(app_name "${app_name}-d")
endif()


if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
