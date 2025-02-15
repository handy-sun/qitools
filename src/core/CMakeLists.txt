cmake_minimum_required(VERSION 3.10)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake")
include(qitools_common)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets LinguistTools)

# if(CMAKE_CXX_COMPILER MATCHES ".*mingw")
#     set(CMAKE_CXX_FLAGS "-mwindows")
# endif()

#message("``` ${CMAKE_CXX_COMPILER_ID}")

#string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id)
#set(app_name "${PROJECT_NAME}-${compiler_id}")

if(WIN32)
    set(SUFFIX ".dll")
elseif(UNIX)
    if(APPLE)
        set(SUFFIX ".dylib")
    else()
        set(SUFFIX ".so")
    endif()
endif()

qt5_add_resources(qrc_file ../../resource/qitools.qrc)
#qt5_add_translation(ts_files ${CMAKE_CURRENT_SOURCE_DIR}/core-zh_CN.ts)
#string(REPLACE " " "\ " win_rc2 ${win_rc})

add_definitions(
    -DQT_MESSAGELOGCONTEXT
    -DQT_DEPRECATED_WARNINGS
    -DDYNAMIC_SUFFIX="${SUFFIX}"
    -DVERSION_STRING="${PROJECT_VERSION}"
)

file(GLOB SRC *.h *.cpp)

if(WIN32)
    # WARNING: 如果路径内有空格，则添加.rc文件编译无法通过
    set(win_rc "${CMAKE_CURRENT_SOURCE_DIR}/../../resource/icon.rc")
    add_executable(${PROJECT_NAME} WIN32 ${SRC} ${qrc_file} ${win_rc} ${ts_files})
elseif(UNIX)
    if(APPLE)
        set(MACOSX_BUNDLE_ICON_FILE "toolsimage.icns")
        set(macos_icns "${EXECUTABLE_OUTPUT_PATH}/../resource/toolsimage.icns")

        if(QITOOLS_MAC_BUNDLE)
            set_source_files_properties(${macos_icns} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
            add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC} ${qrc_file} ${macos_icns} ${ts_files})
        endif()
    else()
        option(QITOOLS_LINUX_RPATH "Use '-Wl,--rpath' to link .so" OFF)
        add_executable(${PROJECT_NAME} ${SRC} ${qrc_file} ${ts_files})

        if(QITOOLS_LINUX_RPATH)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--rpath='\$ORIGIN/../lib'")
            message("``` CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
        endif()
    endif()
endif()

# 设置输出目录相同，无论是 Debug 还是 Release 等
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${EXECUTABLE_OUTPUT_PATH}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${EXECUTABLE_OUTPUT_PATH}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${app_name})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
#定义生成命令
add_custom_target(cptask
    COMMAND test -d "${PLUGIN_PATH}/../translations" || cp -r "${EXECUTABLE_OUTPUT_PATH}/translations" "${PLUGIN_PATH}/../"
    COMMENT "Check translation files..."
)
# add_custom_target(ts DEPENDS ${ts_files})

set(CMAKE_INSTALL_BINDIR "bin")
set(CMAKE_INSTALL_LIBDIR "lib")

install(
    FILES ${EXECUTABLE_OUTPUT_PATH}/${app_name}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#cotire(${PROJECT_NAME})
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(out_list)
    set(_reg_exp "\\.cpp|\\.h")

    get_link_dependencies(${PROJECT_NAME} _deps)
    foreach(_dep IN LISTS _deps)
        get_target_property(_srcs ${_dep} SOURCES)
        foreach(_src IN LISTS _srcs)
            if(_src MATCHES ${_reg_exp})
                list(APPEND out_list "${_src}")
            endif()
        endforeach()
    endforeach()

    ADD_PCH_RULE(src/stable.h out_list)
endif()
