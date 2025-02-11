project("codecconvert" VERSION 3.0.0 LANGUAGES CXX C)

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

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
cmake_policy(SET CMP0048 OLD)

## -------- add ced -------- {{{
set(CED_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ced")
set(CED_LIBRARY_SOURCES
    ${CED_ROOT}/compact_enc_det/compact_enc_det.cc
    ${CED_ROOT}/compact_enc_det/compact_enc_det_hint_code.cc
    ${CED_ROOT}/util/encodings/encodings.cc
    ${CED_ROOT}/util/languages/languages.cc
)
include_directories(${PROJECT_NAME} PUBLIC ${CED_ROOT})
## -------- add ced -------- }}}

add_library(${PROJECT_NAME} SHARED "")

target_sources(${PROJECT_NAME} PRIVATE
    ${CED_LIBRARY_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)

