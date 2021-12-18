project("glsltool" VERSION 0.1.0 LANGUAGES CXX C)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/plugins)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/debug)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets LinguistTools)

add_library(${PROJECT_NAME} SHARED "")

qt5_add_translation(ts_files ${CMAKE_CURRENT_SOURCE_DIR}/glsltool-zh_CN.ts)
#set(ts_files ${${ts_files}} PARENT_SCOPE)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glslcodeconvert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glslcodeconvert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glslcodeconvert.ui
    ${ts_files}
)

#add_custom_target(${PROJECT_NAME} DEPENDS )
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
