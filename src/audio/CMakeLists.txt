project("audio" VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake")
include(qitools_common)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets Multimedia)

add_library(${PROJECT_NAME} SHARED "")

file(GLOB SOURCES *.h *.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

add_subdirectory(minimp3)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Multimedia)
