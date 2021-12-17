cmake_minimum_required(VERSION 3.2)

#project(qitools VERSION 0.10.0 LANGUAGES CXX C)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_MODULE_PATH "{CMAKE_SOURCE_DIR}/../CMake")
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_DEBUG_POSTFIX "_d")

add_subdirectory(codegen)
#add_subdirectory(colortool)
#add_subdirectory(simpledownload)
#add_subdirectory(audio)
#add_subdirectory(imageconvert)
#add_subdirectory(glsltool)
add_subdirectory(codecconvert)
add_subdirectory(core)
