project("colortool" VERSION 1.1.0 LANGUAGES CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake")
include(qitools_common)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets)

add_library(${PROJECT_NAME} SHARED "")

qt5_add_resources(qrc_file ${CMAKE_CURRENT_SOURCE_DIR}/colortool.qrc)

file(GLOB SOURCES *.h *.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${qrc_file})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
