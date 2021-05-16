#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class M5UnitLCD : public lgfx::LGFX_Device
{
  lgfx::Bus_I2C      _bus_instance;
  lgfx::Panel_M5UnitLCD _panel_instance;

public:

  M5UnitLCD(std::uint8_t pin_sda = 21, std::uint8_t pin_scl = 22, std::uint32_t i2c_freq = 400000, std::uint8_t i2c_port = 0, std::uint8_t i2c_addr = 0x3E)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.freq = i2c_freq;
      cfg.freq_read = i2c_freq > 400000 ? 400000 + ((i2c_freq - 400000) >> 1) : i2c_freq;
      //cfg.freq_read = i2c_freq > 400000 ? 400000 : i2c_freq;
      cfg.pin_scl = pin_scl;
      cfg.pin_sda = pin_sda;
      cfg.i2c_port = i2c_port;
      cfg.i2c_addr = i2c_addr;
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
