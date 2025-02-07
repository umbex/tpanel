cmake_minimum_required(VERSION 3.0.0)

# Add library paths
set(EXTRA_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Arduino_GFX-1.4.6/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/lvgl-master"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/TouchLib/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/config"
)

# Define source files for the component
# Includes both our component code and the SquareLine generated UI files
set(COMPONENT_SRCS
    "tpanel_display.cpp"      # Our ESPHome component implementation
    "ui/ui.c"                 # SquareLine generated UI implementation
    "ui/ui_helpers.c"         # SquareLine UI helper functions
)

# Define required components/libraries
set(COMPONENT_REQUIRES
    "esphome"                 # ESPHome core
    "lvgl"                    # LVGL library
    "Arduino_GFX_Library"     # Display driver library
    "TouchLib"                # Touch interface library
)

# ESP-IDF component registration
# This makes our component available to the ESP-IDF build system
idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "." "ui" ${EXTRA_INCLUDE_DIRS}
    REQUIRES ${COMPONENT_REQUIRES}
) 