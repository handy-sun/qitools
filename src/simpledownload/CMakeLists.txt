target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/downvscvsixwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/downvscvsixwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/downvscvsixwidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/networkcontrol.h
    ${CMAKE_CURRENT_SOURCE_DIR}/networkcontrol.cpp
)


