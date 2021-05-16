#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class M5UnitOLED : public lgfx::LGFX_Device
{
  lgfx::Bus_I2C      _bus_instance;
  lgfx::Panel_SH110x _panel_instance;

public:

  M5UnitOLED(std::uint8_t pin_sda = 21, std::uint8_t pin_scl = 22, std::uint32_t i2c_freq = 400000, std::uint8_t i2c_port = 0, std::uint8_t i2c_addr = 0x3C)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.freq = i2c_freq;
      cfg.pin_scl = pin_scl;
      cfg.pin_sda = pin_sda;
      cfg.i2c_port = i2c_port;       // I2C_NUM_0;
      cfg.i2c_addr = i2c_addr;
      cfg.prefix_cmd = 0x00;
      cfg.prefix_data = 0x40;
      cfg.prefix_len = 1;

      _bus_instance.config(cfg);
      _panel_instance.bus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.panel_width  =  64;
      cfg.panel_height = 128;
      cfg.offset_x     =  32;
      _panel_instance.config(cfg);
    }

    setPanel(&_panel_instance);
  }
};
