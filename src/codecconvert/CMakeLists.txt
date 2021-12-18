project("codecconvert" VERSION 1.0.0 LANGUAGES CXX C)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/plugins)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/debug)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets)

add_library(${PROJECT_NAME} SHARED "")

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.ui
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
