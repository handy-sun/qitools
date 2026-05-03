project("imageconvert" VERSION 1.0.0 LANGUAGES CXX C)

set(LIBRARY_OUTPUT_PATH "${PLUGIN_PATH}")
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/debug)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(qt_compatibility)
find_package(${QT_LIBS_PREFIX} CONFIG REQUIRED Core Gui Widgets)

add_library(${PROJECT_NAME} SHARED "")

file(GLOB SOURCES *.h *.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ${QT_LIBS_PREFIX}::Widgets ${QT_LIBS_PREFIX}::Core ${QT_LIBS_PREFIX}::Gui)
