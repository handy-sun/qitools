project("glsltool" VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake")
include(qitools_common)

include(qt_compatibility)
find_package(${QT_LIBS_PREFIX} CONFIG REQUIRED Core Gui Widgets LinguistTools)

add_library(${PROJECT_NAME} SHARED "")

qt_add_translation(ts_files ${CMAKE_CURRENT_SOURCE_DIR}/glsltool-zh_CN.ts)
#set(ts_files ${${ts_files}} PARENT_SCOPE)

file(GLOB SOURCES *.h *.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${ts_files})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ${QT_LIBS_PREFIX}::Widgets ${QT_LIBS_PREFIX}::Core ${QT_LIBS_PREFIX}::Gui)
