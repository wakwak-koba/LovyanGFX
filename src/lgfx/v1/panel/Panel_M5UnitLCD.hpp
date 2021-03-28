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

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  class Panel_M5UnitLCD : public Panel_Device
  {
  public:
    Panel_M5UnitLCD(void)
    {
      _cfg.memory_width  = _cfg.panel_width = 135;
      _cfg.memory_height = _cfg.panel_height = 240;
    }

    void beginTransaction(void) override;
    void endTransaction(void) override;

    color_depth_t setColorDepth(color_depth_t depth) override;
    void setRotation(std::uint_fast8_t r) override;
    void setBrightness(std::uint8_t brightness) override;
    void setInvert(bool invert) override;
    void setSleep(bool flg) override {}
    void setPowerSave(bool flg) override {}

    void waitDisplay(void) override {}
    bool displayBusy(void) override { return false; }
    void display(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h) override {}

    void writePixels(pixelcopy_t* param, std::uint32_t len) override {}
    void writeBlock(std::uint32_t rawcolor, std::uint32_t len) override;

    void setWindow(std::uint_fast16_t xs, std::uint_fast16_t ys, std::uint_fast16_t xe, std::uint_fast16_t ye) override;
    void drawPixelPreclipped(std::uint_fast16_t x, std::uint_fast16_t y, std::uint32_t rawcolor) override;
    void writeFillRectPreclipped(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, std::uint32_t rawcolor) override;
    void writeImage(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;

    std::uint32_t readCommand(std::uint_fast8_t cmd, std::uint_fast8_t index, std::uint_fast8_t len) override { return 0; }
    std::uint32_t readData(std::uint_fast8_t index, std::uint_fast8_t len) override { return 0; }
    void readRect(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, void* dst, pixelcopy_t* param) override {}

    static constexpr std::uint8_t CMD_NOP          = 0x00; // 1byte 何もしない
    static constexpr std::uint8_t CMD_RESET        = 0x01; // 1byte ソフトリセット
    static constexpr std::uint8_t CMD_INVOFF       = 0x20; // 1byte 色反転を解除
    static constexpr std::uint8_t CMD_INVON        = 0x21; // 1byte 色反転を有効
    static constexpr std::uint8_t CMD_BRIGHTNESS   = 0x22; // 2byte バックライト data[1]==明るさ 0~255
    static constexpr std::uint8_t CMD_DISPLAY_OFF  = 0x28; // 1byte 画面消灯
    static constexpr std::uint8_t CMD_DISPLAY_ON   = 0x29; // 1byte 画面点灯
    static constexpr std::uint8_t CMD_CASET        = 0x2A; // 5byte X方向の範囲選択 data[1~2]==XS  data[3~4]==XE
    static constexpr std::uint8_t CMD_RASET        = 0x2B; // 5byte Y方向の範囲選択 data[1~2]==YS  data[3~4]==YE

    static constexpr std::uint8_t CMD_WR_RAW       = 0x40;
    static constexpr std::uint8_t CMD_WR_RAW_8     = 0x41; // 不定長 RGB332のピクセルデータを連続送信
    static constexpr std::uint8_t CMD_WR_RAW_16    = 0x42; // 不定長 RGB565のピクセルデータを連続送信
    static constexpr std::uint8_t CMD_WR_RAW_24    = 0x43; // 不定長 RGB888のピクセルデータを連続送信
    static constexpr std::uint8_t CMD_WR_RLE       = 0x44;
    static constexpr std::uint8_t CMD_WR_RLE_8     = 0x45; // 不定長 RGB332のRLE圧縮されたピクセルデータを連続送信
    static constexpr std::uint8_t CMD_WR_RLE_16    = 0x46; // 不定長 RGB565のRLE圧縮されたピクセルデータを連続送信
    static constexpr std::uint8_t CMD_WR_RLE_24    = 0x47; // 不定長 RGB888のRLE圧縮されたピクセルデータを連続送信

    static constexpr std::uint8_t CMD_RAM_FILL     = 0x50; // 1byte 現在の描画色で選択範囲全塗り
    static constexpr std::uint8_t CMD_SET_COLOR    = 0x50;
    static constexpr std::uint8_t CMD_SET_COLOR_8  = 0x51; // 2byte 描画色をRGB332で指定
    static constexpr std::uint8_t CMD_SET_COLOR_16 = 0x52; // 3byte 描画色をRGB565で指定
    static constexpr std::uint8_t CMD_SET_COLOR_24 = 0x53; // 4byte 描画色をRGB888で指定

//    static constexpr std::uint8_t CMD_SET_COLOR    = 0x3F; // 3byte 描画色設定 data[1~2]==RGB565色データ
//    static constexpr std::uint8_t CMD_RAMWR      = 0x40; // 不定長 ピクセルデータ送信
//    static constexpr std::uint8_t CMD_RAM_RLE    = 0x41; // 不定長 RLEピクセルデータ送信
//    static constexpr std::uint8_t CMD_COLMOD     = 0x41;

  protected:
  
    std::uint32_t _raw_color = ~0u;
    std::uint32_t _xs_raw, _xe_raw, _ys_raw, _ye_raw;
    std::uint32_t _xpos;
    std::uint32_t _ypos;

    void _set_window(std::uint_fast16_t xs, std::uint_fast16_t ys, std::uint_fast16_t xe, std::uint_fast16_t ye);
    void _update_colmod(void);
  };

//----------------------------------------------------------------------------
 }
}
