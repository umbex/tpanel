#include "tpanel_display.h"
#include "lib/config/pin_config.h"
#include "lib/config/lv_conf.h"

namespace esphome {
namespace tpanel {

static volatile bool Touch_Int_Flag = false;

void TPanelDisplay::setup() {
  // Initialize display backlight
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  // Setup touch interrupt
  attachInterrupt(
      TOUCH_INT,
      []() {
          Touch_Int_Flag = true;
      },
      FALLING);

  Wire.begin(IIC_SDA, IIC_SCL);

  // Initialize display
  Arduino_DataBus *bus = new Arduino_XL9535SWSPI(IIC_SDA, IIC_SCL, -1,
                                             XL95X5_CS, XL95X5_SCLK, XL95X5_MOSI);
  
  this->rgbpanel = new Arduino_ESP32RGBPanel(
      -1, LCD_VSYNC, LCD_HSYNC, LCD_PCLK,
      LCD_B0, LCD_B1, LCD_B2, LCD_B3, LCD_B4,
      LCD_G0, LCD_G1, LCD_G2, LCD_G3, LCD_G4, LCD_G5,
      LCD_R0, LCD_R1, LCD_R2, LCD_R3, LCD_R4,
      1, 20, 2, 0,
      1, 30, 8, 1,
      10, 6000000L, false,
      0, 0);

  this->gfx = new Arduino_RGB_Display(
      LCD_WIDTH, LCD_HEIGHT, rgbpanel, 0, true,
      bus, -1, st7701_type9_init_operations, sizeof(st7701_type9_init_operations));

  this->gfx->begin();
  this->gfx->fillScreen(BLACK);

  // Initialize touch
  this->gfx->XL_digitalWrite(XL95X5_TOUCH_RST, LOW);
  delay(200);
  this->gfx->XL_digitalWrite(XL95X5_TOUCH_RST, HIGH);
  delay(200);
  
  this->touch = new TouchLib(Wire, TOUCH_SDA, TOUCH_SCL, CST3240_ADDRESS);
  this->touch->init();

  // Initialize LVGL
  this->lvgl_initialization();
}

void TPanelDisplay::loop() {
  lv_timer_handler();
  
  if (this->writer_ != nullptr) {
    this->writer_(this);
  }
}

void TPanelDisplay::dump_config() {
  ESP_LOGCONFIG("TPanelDisplay", "TPanelDisplay initialized");
}

void TPanelDisplay::lvgl_initialization() {
  lv_init();

  // Initialize display buffer
  this->buf1_ = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  this->buf2_ = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

  lv_disp_draw_buf_init(&this->draw_buf_, this->buf1_, this->buf2_, LCD_WIDTH * 40);

  // Initialize display driver
  lv_disp_drv_init(&this->disp_drv_);
  this->disp_drv_.hor_res = LCD_WIDTH;
  this->disp_drv_.ver_res = LCD_HEIGHT;
  this->disp_drv_.flush_cb = my_disp_flush;
  this->disp_drv_.draw_buf = &this->draw_buf_;
  this->disp_drv_.user_data = this;
  lv_disp_drv_register(&this->disp_drv_);

  // Initialize touch input
  lv_indev_drv_init(&this->indev_drv_);
  this->indev_drv_.type = LV_INDEV_TYPE_POINTER;
  this->indev_drv_.read_cb = my_touchpad_read;
  this->indev_drv_.user_data = this;
  lv_indev_drv_register(&this->indev_drv_);

  // Initialize the UI
  ui_init();
}

void TPanelDisplay::my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  auto *driver = (TPanelDisplay *)disp->user_data;
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  driver->gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  driver->gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void TPanelDisplay::my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  auto *driver = (TPanelDisplay *)indev_drv->user_data;
  
  if (Touch_Int_Flag) {
    driver->touch->read();
    TP_Point t = driver->touch->getPoint(0);

    if ((driver->touch->getPointNum() == 1) && (t.pressure > 0) && (t.state != 0)) {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = t.x;
      data->point.y = t.y;
    }
    Touch_Int_Flag = false;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

}  // namespace tpanel
}  // namespace esphome 