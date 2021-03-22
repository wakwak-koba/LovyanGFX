/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include "Panel_Device.hpp"
#include "../misc/range.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_SSD1306 : public Panel_Device
  {
    Panel_SSD1306(void) : Panel_Device()
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 64;
      _cfg.dummy_read_bits = 0;
      _auto_display = true;
    }

    virtual ~Panel_SSD1306(void);

    void beginTransaction(void) override;
    void endTransaction(void) override;
    void init(bool use_reset) override;

    color_depth_t setColorDepth(color_depth_t depth) override;

    void setRotation(std::uint_fast8_t r) override;
    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool flg) override {}

    void waitDisplay(void) override;
    bool displayBusy(void) override;
    void display(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h) override;

    void writeBlock(std::uint32_t rawcolor, std::uint32_t len) override;
    void setWindow(std::uint_fast16_t xs, std::uint_fast16_t ys, std::uint_fast16_t xe, std::uint_fast16_t ye) override;
    void drawPixelPreclipped(std::uint_fast16_t x, std::uint_fast16_t y, std::uint32_t rawcolor) override;
    void writeFillRectPreclipped(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, std::uint32_t rawcolor) override;
    void writeImage(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writePixels(pixelcopy_t* param, std::uint32_t len) override;

    std::uint32_t readCommand(std::uint_fast8_t cmd, std::uint_fast8_t index = 0, std::uint_fast8_t length = 4) override { return 0; }
    std::uint32_t readData(std::uint_fast8_t index = 0, std::uint_fast8_t length = 4) override { return 0; }

    void readRect(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, void* dst, pixelcopy_t* param) override;

  private:

    std::uint8_t* _buf = nullptr;

    rect16_t _range_new;
    std::int32_t _xpos = 0;
    std::int32_t _ypos = 0;

    void _draw_pixel(std::int32_t x, std::int32_t y, std::uint32_t value);
    bool _read_pixel(std::int32_t x, std::int32_t y);
    void _update_transferred_rect(std::uint32_t &xs, std::uint32_t &ys, std::uint32_t &xe, std::uint32_t &ye);

    static constexpr std::uint8_t CMD_MEMORYMODE = 0x20;

    static constexpr std::uint8_t CMD_COLUMNADDR = 0x21;
    static constexpr std::uint8_t CMD_PAGEADDR   = 0x22;

    static constexpr std::uint8_t CMD_SETSTARTLINE= 0x40;
    static constexpr std::uint8_t CMD_DISP_OFF   = 0xAE;
    static constexpr std::uint8_t CMD_DISP_ON    = 0xAF;
    static constexpr std::uint8_t CMD_SETCONTRAST= 0x81;
    static constexpr std::uint8_t CMD_CHARGEPUMP = 0x8D;
    static constexpr std::uint8_t CMD_SEGREMAP   = 0xA0;
    static constexpr std::uint8_t CMD_SETMLTPLX  = 0xA8;
    static constexpr std::uint8_t CMD_COMSCANDEC = 0xC8;
    static constexpr std::uint8_t CMD_SETOFFSET  = 0xD3;
    static constexpr std::uint8_t CMD_SETCLKDIV  = 0xD5;
    static constexpr std::uint8_t CMD_SETPRECHG  = 0xD9;
    static constexpr std::uint8_t CMD_SETCOMPINS = 0xDA;
    static constexpr std::uint8_t CMD_SETVCOMDET = 0xDB;

    static constexpr std::uint8_t CMD_DISPLAYALLON_RESUME = 0xA4;
    static constexpr std::uint8_t CMD_NORMALDISPLAY = 0xA6;
    static constexpr std::uint8_t CMD_INVERTDISPLAY = 0xA7;
    static constexpr std::uint8_t CMD_DEACTIVATE_SCROLL = 0x2E;

    const std::uint8_t* getInitCommands(std::uint8_t listno) const override
    {
      static constexpr std::uint8_t list0[] = {
          CMD_DISP_OFF   , 0,
          CMD_SETCLKDIV  , 0, 0x80, 0,
          CMD_SETMLTPLX  , 0, 0x3F, 0,
          CMD_SETOFFSET  , 0, 0x00, 0,
          CMD_SETSTARTLINE, 0,
          CMD_MEMORYMODE , 0, 0x00, 0,
          CMD_SEGREMAP|1 , 0,
          CMD_COMSCANDEC , 0,
          CMD_SETCOMPINS , 0, 0x12, 0,
          CMD_SETVCOMDET , 0, 0x40, 0,
          CMD_DISPLAYALLON_RESUME, 0,
          CMD_DEACTIVATE_SCROLL, 0,
          CMD_DISP_ON    , 0,
          0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

//----------------------------------------------------------------------------
 }
}
