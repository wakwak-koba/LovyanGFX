#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class M5UnitLCD : public lgfx::LGFX_Device
{
  lgfx::Bus_I2C      _bus_instance;
  lgfx::Panel_M5UnitLCD _panel_instance;

public:

  M5UnitLCD(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.freq = 400000;
      cfg.pin_scl = 22;
      cfg.pin_sda = 21;
      cfg.i2c_port = 0;
      cfg.i2c_addr = 0x3C;
      cfg.prefix_len = 0;

      _bus_instance.config(cfg);
      _panel_instance.bus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      cfg.memory_width     = 135;
      cfg.memory_height    = 240;
      cfg.panel_width      = 135;
      cfg.panel_height     = 240;
      cfg.offset_x         =   0;
      cfg.offset_rotation  =   0;

      _panel_instance.config(cfg);
    }

    setPanel(&_panel_instance);
  }
};
