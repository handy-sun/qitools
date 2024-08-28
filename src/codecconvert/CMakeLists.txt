project("codecconvert" VERSION 2.0.0 LANGUAGES CXX C)

set(LIBRARY_OUTPUT_PATH "${PLUGIN_PATH}")
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/debug)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets)

set(DISABLE_WARNING "-Wno-missing-field-initializers -Wno-unused-const-variable -Wno-tautological-overlap-compare -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DISABLE_WARNING}")

set(BUILD_BINARY OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC ON)
add_subdirectory(uchardet)

add_library(${PROJECT_NAME} SHARED "")

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.ui
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui libuchardet_static)
