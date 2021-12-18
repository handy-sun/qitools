cmake_minimum_required(VERSION 3.2.0)

set(PROJECT_AUTHOR "sooncheer")
project("qitools" VERSION 0.10.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../CMake")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin)
set(CMAKE_DEBUG_POSTFIX "_d")

# 本次使用C++标准版本
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets LinguistTools)

if(CMAKE_CXX_COMPILER MATCHES ".*mingw")
    set(CMAKE_CXX_FLAGS "-mwindows")
endif()

#message("``` ${CMAKE_CXX_COMPILER_ID}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    include(pch_gcc4)
endif()

#include(cotire)
#include(get_depends)

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id)
set(app_name "${PROJECT_NAME}-${compiler_id}")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(app_name "${app_name}-d")
endif()

if(WIN32)
    set(SUFFIX ".dll")
elseif(UNIX)
    if(APPLE)
        set(SUFFIX ".dylib")
    else()
        set(SUFFIX ".so")
    endif()
endif()

qt5_add_resources(qrc_file ../../resource/qitools.qrc)
#qt5_add_translation(ts_files ${CMAKE_CURRENT_SOURCE_DIR}/core-zh_CN.ts)
#string(REPLACE " " "\ " win_rc2 ${win_rc})

set(SRC
    main.cpp 
    pluginmanager.cpp 
    qitoolswindow.cpp 
    pluginmanager.h 
    pluginterface.h 
    qitoolswindow.h 
    stable.h
)

add_definitions(
    -DQT_MESSAGELOGCONTEXT
    -DQT_DEPRECATED_WARNINGS
    -DDYNAMIC_SUFFIX="${SUFFIX}"
    -DVERSION_STRING="${PROJECT_VERSION}"
)

if(WIN32)
    set(win_rc "${CMAKE_SOURCE_DIR}/../resource/icon.rc") # PS: 如果路径内有空格，则添加.rc文件编译无法通过
    add_executable(${PROJECT_NAME} WIN32 ${SRC} ${qrc_file} ${win_rc} ${ts_files})
else()
    add_executable(${PROJECT_NAME} ${SRC} ${qrc_file} ${win_rc} ${ts_files})
endif()

#add_custom_target(ts DEPENDS ${ts_files})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${app_name})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)

#cotire(${PROJECT_NAME})

#set(out_list )
#set(_reg_exp "\\.cpp|\\.h")

#get_link_dependencies(${PROJECT_NAME} _deps)
#foreach(_dep IN LISTS _deps)
#    get_target_property(_srcs ${_dep} SOURCES)
#    foreach(_src IN LISTS _srcs)
#        if(_src MATCHES ${_reg_exp})
#            list(APPEND out_list "${_src}")
#        endif()
#    endforeach()
#endforeach()

#ADD_PCH_RULE(src/stable.h out_list)
