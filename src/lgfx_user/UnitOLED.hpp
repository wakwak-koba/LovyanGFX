#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class M5UnitOLED : public lgfx::LGFX_Device
{
  lgfx::Bus_I2C      _bus_instance;
  lgfx::Panel_SH110x _panel_instance;

public:

  M5UnitOLED(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.freq = 400000;
      cfg.pin_scl = 22;
      cfg.pin_sda = 21;
      cfg.i2c_port = 0;       // I2C_NUM_0;
      cfg.i2c_addr = 0x3C;
      cfg.prefix_cmd = 0x00;
      cfg.prefix_data = 0x40;
      cfg.prefix_len = 1;

      _bus_instance.config(cfg);
      _panel_instance.bus(&_bus_instance);
    }

    setPanel(&_panel_instance);
  }
};
