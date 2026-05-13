
set(LIBRARY_OUTPUT_PATH "${PLUGIN_PATH}")

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/debug)
endif()

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_PATH}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PLUGIN_PATH}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PLUGIN_PATH}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PLUGIN_PATH}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PLUGIN_PATH}")
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
