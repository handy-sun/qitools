
set(LIBRARY_OUTPUT_PATH "${PLUGIN_PATH}")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/debug)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # message("common.msvc")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(DISABLE_WARNING "-Wno-missing-field-initializers -Wno-unused-const-variable -Wno-tautological-overlap-compare -Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DISABLE_WARNING}")
    include(pch_gcc4)
    include(get_depends)
endif()