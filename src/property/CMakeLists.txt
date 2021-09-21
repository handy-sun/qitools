target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/propertygenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propertygenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/propertygenerator.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/propertymanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propertymanager.cpp
)
