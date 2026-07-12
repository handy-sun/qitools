if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.16)
    project(qitools_tests LANGUAGES CXX)
    set(CMAKE_AUTOMOC ON)
    enable_testing()
    find_package(Qt6 CONFIG REQUIRED Core Test)
    set(QT_LIBS_PREFIX Qt6)
else()
    include(qt_compatibility)
endif()

set(CMAKE_AUTOMOC ON)
find_package(${QT_LIBS_PREFIX} CONFIG REQUIRED Core Test)

add_executable(tst_sourcefilefilter
    tst_sourcefilefilter.cpp
    ../codecconvert/sourcefilefilter.cpp
)

target_include_directories(tst_sourcefilefilter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../codecconvert
)

target_link_libraries(tst_sourcefilefilter PRIVATE
    ${QT_LIBS_PREFIX}::Core
    ${QT_LIBS_PREFIX}::Test
)

add_test(NAME sourcefilefilter COMMAND tst_sourcefilefilter)
