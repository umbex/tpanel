# ESPHome component configuration entry point
# This file defines how the component appears in YAML configurations

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_LAMBDA

# External library dependencies
ARDUINO_GFX_LIB_VERSION = "1.5.3"

# Create a C++ namespace for our component
tpanel_ns = cg.esphome_ns.namespace("tpanel")
TPanelDisplay = tpanel_ns.class_("TPanelDisplay", cg.Component)

# Define the YAML schema for our component
# This allows users to configure the component in their YAML files
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(TPanelDisplay),
    cv.Optional(CONF_LAMBDA): cv.lambda_,  # Optional lambda for runtime UI updates
}).extend(cv.COMPONENT_SCHEMA)

# Code generation for ESPHome
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Add framework dependencies
    cg.add_platformio_option("framework", "arduino")
    
    # Add build flags
    cg.add_build_flag("-DESP32")
    cg.add_build_flag("-DARDUINO_ARCH_ESP32")
    
    # Add include paths for local libraries
    cg.add_build_flag("-I$PROJECT_DIR/lib")  # Use absolute path
    
    # Add core libraries
    cg.add_library("Wire", None)
    cg.add_library("SPI", None)
    
    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], 
            [(TPanelDisplay.operator("ptr"), "it")],
            return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

CODEOWNERS = ["@jesserockz"]
DEPENDENCIES = ["esp32"]
AUTO_LOAD = ["sensor"]

# Add library dependencies path
LIB_DEPS = [
    "https://github.com/moononournation/Arduino_GFX.git#v1.4.6",
    "https://github.com/lvgl/lvgl.git#v8.3.9",
    "https://github.com/Xinyuan-LilyGO/TouchLib.git",
] 