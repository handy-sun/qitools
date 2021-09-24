target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qitoolswindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qitoolswindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qitoolswindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/stable.h
)

add_subdirectory(audio)
add_subdirectory(codecconvert)
add_subdirectory(color)
add_subdirectory(glsltool)
add_subdirectory(imageconvert)
add_subdirectory(property)
add_subdirectory(vsc_vsix)
