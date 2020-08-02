cmake_minimum_required(VERSION 3.2.0)

#set(CMAKE_C_COMPILER "D:\\Qt\\Qt5.9.2\\Tools\\mingw530_32\\bin\\gcc.exe")
#set(CMAKE_CXX_COMPILER "D:\\Qt\\Qt5.9.2\\Tools\\mingw530_32\\bin\\g++.exe")
# Point CMake to the custom modules
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# 本次使用C++标准版本
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
# It would be nice if CMake offered a constant for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX 1)
endif()

set(PROJECT_NAME "QiTools")
set(PROJECT_AUTHOR "SunQi")
project(${PROJECT_NAME} CXX)

#set(AUTO_RECURSE_FILE)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#打开全局rcc，非必须，
set(CMAKE_AUTORCC ON)

# 自动搜索各种文件，易出现重复定义问题
if(AUTO_RECURSE_FILE)
    file(GLOB_RECURSE SOURCES "*.cpp")
    file(GLOB_RECURSE HEADERS "*.h")
    file(GLOB_RECURSE FORMS "*.ui")
#    file(GLOB_RECURSE RESOURCES "*.qrc")
    file(GLOB_RECURSE all_files *.h *.cpp *.c *.cc *.hpp) #create_filters(all_files) unknown cmake command?

    #通过Ui文件生成对应的头文件，一定要添加 ???
    #qt5_wrap_ui(WRAP_FILES ${FORMS})

    #将ui文件和生成文件整理在一个文件夹中，非必须
    #source_group("Ui" FILES ${UI_FILES} ${WRAP_FILES} )

    #list(REMOVE_ITEM SOURCES "CMakeCXXCompilerId.cpp")
    #list(REMOVE_ITEM SOURCES "CMakeCXX*.cpp")
    #list(REMOVE_ITEM HEADERS "moc*.h")
else()
    set(SOURCES
        src/main.cpp
        src/qitoolswindow.cpp
        src/qitoolswindow.h
        src/stable.h
        src/qitoolswindow.ui

        src/property/propertygenerator.cpp
        src/property/propertymanager.cpp
        src/property/propertygenerator.ui
        src/property/propertygenerator.h
        src/property/propertymanager.h

        src/color/colorconvert.h
        src/color/colorconvert.cpp
        src/color/colorconvert.ui
        src/color/screencolorpicker.h
        src/color/screencolorpicker.cpp
        )
endif()


if(CMAKE_CL_64)
    set(EXECUTABLE_OUTPUT_PATH ../binX64)
    set(ADDRESS_MODEL 64)
    set(NODE_TARGET x64)
else(CMAKE_CL_64)
    set(EXECUTABLE_OUTPUT_PATH ../binX86)
    set(ADDRESS_MODEL 32)
    set(NODE_TARGET x86)
endif(CMAKE_CL_64)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(EXE_NAME "${PROJECT_NAME}_D")
else()
    set(EXE_NAME ${PROJECT_NAME})
endif()

include_directories(src src/property src/color)

qt5_add_resources(QRC_FILES resource/qitools.qrc)


if(WIN32)
    set(WIN_RC "${CMAKE_CURRENT_SOURCE_DIR}/resource/icon.rc")
endif()
#string(REPLACE " " "\ " WIN_RC2 ${WIN_RC})
message(">>>${WIN_RC}")


if(AUTO_RECURSE_FILE)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${QRC_FILES}
#    ${WRAP_FILES}
        ${WIN_RC}
    )
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${QRC_FILES}
#        ${WIN_RC}
        )
endif()
# PS: 如果路径内有空格，则添加.rc文件编译无法通过

set_target_properties(${PROJECT_NAME} PROPERTIES
                      OUTPUT_NAME ${EXE_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
