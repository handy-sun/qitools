project("codecconvert" VERSION 3.0.0 LANGUAGES CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake")
include(qitools_common)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets)


set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
# cmake_policy(SET CMP0048 OLD)

## -------- add ced -------- {{{
set(CED_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ced")
set(CED_SOURCES
    ${CED_ROOT}/compact_enc_det/compact_enc_det.cc
    ${CED_ROOT}/compact_enc_det/compact_enc_det_hint_code.cc
    ${CED_ROOT}/util/encodings/encodings.cc
    ${CED_ROOT}/util/languages/languages.cc
)
include_directories(${PROJECT_NAME} PUBLIC ${CED_ROOT})
## -------- add ced -------- }}}

add_library(${PROJECT_NAME} SHARED "")

file(GLOB SOURCES *.h *.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${CED_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
