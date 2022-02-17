cmake_minimum_required(VERSION 3.2)

set(PROJECT_AUTHOR "sooncheer")
project("qitools" VERSION 0.12.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../CMake")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
#set(CMAKE_DEBUG_POSTFIX "_d")

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id)
set(app_name "${PROJECT_NAME}-${compiler_id}")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(app_name "${app_name}-d")
endif()

if(APPLE)
    set(PLUGIN_PATH "${EXECUTABLE_OUTPUT_PATH}/${app_name}.app/Contents/MacOS/plugins")
else()
    set(PLUGIN_PATH "${EXECUTABLE_OUTPUT_PATH}/plugins")
endif()

add_subdirectory(codegen)
add_subdirectory(colortool)
add_subdirectory(simpledownload)
add_subdirectory(audio)
add_subdirectory(imageconvert)
add_subdirectory(glsltool)
add_subdirectory(codecconvert)
add_subdirectory(core)
