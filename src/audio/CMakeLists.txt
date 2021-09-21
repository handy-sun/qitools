target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/audiodataplay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/audiodataplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audiowidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/audiowidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audiowidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/wavfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/wavfile.cpp
)
