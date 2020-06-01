cmake_minimum_required(VERSION 3.2.0)

#set(CMAKE_C_COMPILER "D:\\Qt\\Qt5.9.2\\Tools\\mingw530_32\\bin\\gcc.exe")
#set(CMAKE_CXX_COMPILER "D:\\Qt\\Qt5.9.2\\Tools\\mingw530_32\\bin\\g++.exe")
# Point CMake to the custom modules
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# 本次使用C++标准版本
set(CMAKE_CXX_STANDARD 11)

# Set appropriate compiler flags, etc. (MSVC is already set to W3 and Clang
# issues enough warnings as it is)

if(CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# It would be nice if CMake offered a constant for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX 1)
endif()

set(PROJECT_NAME "QiTools")
set(PROJECT_AUTHOR "SunQi")
project(${PROJECT_NAME} CXX)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# At the very minimum, Qt 5.9+ is required
#find_package(Qt5Gui 5.9)
#find_package(Qt5Core 5.9)
#find_package(Qt5Widgets 5.9)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#打开全局rcc，非必须，
# set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")

file(GLOB_RECURSE all_files *.h *.cpp *.c *.cc *.hpp)
#create_filters(all_files)

#通过Ui文件生成对应的头文件，一定要添加
qt5_wrap_ui(WRAP_FILES ${FORMS})

#添加资源文件，非必须，一旦采用，注意修改相应的qrc文件名
#set(RCC_FILES Qto.qrc)

#将ui文件和生成文件整理在一个文件夹中，非必须
#source_group("Ui" FILES ${UI_FILES} ${WRAP_FILES} )

if(CMAKE_CL_64)
    set(EXECUTABLE_OUTPUT_PATH ../out_X64)
else(CMAKE_CL_64)
    set(EXECUTABLE_OUTPUT_PATH ../out_X86)
endif(CMAKE_CL_64)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(EXE_NAME "${PROJECT_NAME}_D")
else()
    set(EXE_NAME ${PROJECT_NAME})
endif()

list(REMOVE_ITEM SOURCES "CMakeCXXCompilerId.cpp")
#list(REMOVE_ITEM SOURCES "CMakeCXX*.cpp")
#list(REMOVE_ITEM HEADERS "moc*.h")
message(${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS}
    ${RESOURCES} ${WRAP_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
