#pragma once

// ESPHome core includes
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

// Touch module configuration
#define TOUCH_MODULES_CST_MUTUAL

// External library includes
#include <Arduino.h>
#include <SPI.h>
#include "Arduino_GFX-1.4.6/src/Arduino_GFX.h"  // Direct path to main header
#include "lvgl-master/src/lvgl.h"               // Direct path to LVGL
#include "TouchLib/src/TouchLib.h"              // Direct path to TouchLib

// Include the SquareLine generated UI
#include "ui/ui.h"

namespace esphome {
namespace tpanel {

// Main component class - inherits from ESPHome's Component base class
class TPanelDisplay : public Component {
 public:
  // ESPHome lifecycle methods
  void setup() override;  // Called once at startup
  void loop() override;   // Called repeatedly in main loop
  void dump_config() override;  // Called to log component configuration
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

  // Lambda handler for runtime UI updates from YAML
  void set_writer(std::function<void(TPanelDisplay *)> &&writer) { this->writer_ = writer; }
  
 protected:
  // LVGL callbacks
  static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
  static void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
  void lvgl_initialization();

  // Member variables
  std::function<void(TPanelDisplay *)> writer_;  // Stores the YAML lambda
  Arduino_ESP32RGBPanel *rgbpanel{nullptr};      // Hardware display panel driver
  Arduino_RGB_Display *gfx{nullptr};             // Graphics interface
  TouchLib *touch{nullptr};                      // Touch interface
  
  // LVGL display buffers and drivers
  lv_disp_draw_buf_t draw_buf_;
  lv_disp_drv_t disp_drv_;
  lv_indev_drv_t indev_drv_;
  lv_color_t *buf1_{nullptr};
  lv_color_t *buf2_{nullptr};
};

}  // namespace tpanel
}  // namespace esphome 