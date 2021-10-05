cmake_minimum_required(VERSION 3.2.0)

set(PROJECT_NAME "QiTools")
set(PROJECT_AUTHOR "SunQi")
project(${PROJECT_NAME} CXX)

#if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#    set(_linux 1)
#endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 本次使用C++标准版本
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#打开全局rcc，非必须，
set(CMAKE_AUTORCC ON)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets Network Multimedia)
#find_package(Threads)

#if(CMAKE_CL_64)
#    set(ADDRESS_MODEL 64)
#    set(NODE_TARGET x64)
#else(CMAKE_CL_64)
#    set(ADDRESS_MODEL 32)
#    set(NODE_TARGET x86)
#endif(CMAKE_CL_64)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    include(pch_gcc4)
endif()
include(cotire)
include(get_depends)

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id)
set(app_name "${PROJECT_NAME}_${compiler_id}")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(app_name "${app_name}_D")
endif()

qt5_add_resources(qrc_file resource/qitools.qrc)

if(WIN32)
    set(win_rc "${CMAKE_SOURCE_DIR}/resource/icon.rc")
endif()
#string(REPLACE " " "\ " win_rc2 ${win_rc})


# PS: 如果路径内有空格，则添加.rc文件编译无法通过
add_executable(${PROJECT_NAME} WIN32 ${qrc_file} ${win_rc})

add_subdirectory(src)
add_subdirectory(third_party)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${app_name})
#${CMAKE_THREAD_LIBS_INIT}
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Multimedia)
cotire(${PROJECT_NAME})

set(out_list )
set(_reg_exp "\\.cpp|\\.h")

get_link_dependencies(${PROJECT_NAME} _deps)
foreach(_dep IN LISTS _deps)
    get_target_property(_srcs ${_dep} SOURCES)
    foreach(_src IN LISTS _srcs)
        if(_src MATCHES ${_reg_exp})
            list(APPEND out_list "${_src}")
        endif()
    endforeach()
endforeach()

#ADD_PCH_RULE(src/stable.h out_list)
