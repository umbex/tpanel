cmake_minimum_required(VERSION 3.16)

# Set component path
set(COMPONENT_PATH ${CMAKE_CURRENT_LIST_DIR})

# Include directories
include_directories(
    ${COMPONENT_PATH}/lib
    ${COMPONENT_PATH}/lib/Arduino_GFX-1.4.6/src
    ${COMPONENT_PATH}/lib/lvgl-master/src
    ${COMPONENT_PATH}/lib/TouchLib/src
)

# Add source files
set(COMPONENT_SRCS
    "tpanel_display.cpp"
)

# Set component requirements
set(COMPONENT_REQUIRES
    "esphome"
    "arduino"
)

# Register component
idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "."
    REQUIRES ${COMPONENT_REQUIRES}
) 