target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/imagetoolwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imagetoolwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imagetoolwidget.ui
)
