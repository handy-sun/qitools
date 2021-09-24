target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codecconvertwidget.ui
)
