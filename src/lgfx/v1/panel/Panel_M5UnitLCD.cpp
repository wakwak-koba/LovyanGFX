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
#include "Panel_M5UnitLCD.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"

#include <Arduino.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void Panel_M5UnitLCD::beginTransaction(void)
  {
    _bus->beginTransaction();
    cs_control(false);
  }

  void Panel_M5UnitLCD::endTransaction(void)
  {
    _bus->endTransaction();
    cs_control(true);
  }

  color_depth_t Panel_M5UnitLCD::setColorDepth(color_depth_t depth)
  {
    auto bits = (depth & color_depth_t::bit_mask);
    if      (bits > 16) { depth = color_depth_t::rgb888_3Byte; }
    else if (bits < 16) { depth = color_depth_t::rgb332_1Byte; }
    else                { depth = color_depth_t::rgb565_2Byte; }

    _read_depth = _write_depth = depth;
    _read_bits  = _write_bits  = depth & color_depth_t::bit_mask;
//    _update_colmod();
    return _write_depth;
  }

  void Panel_M5UnitLCD::setRotation(std::uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));

    auto pw = _cfg.panel_width;
    auto ph = _cfg.panel_height;
    auto mw = _cfg.memory_width;
    auto mh = _cfg.memory_height;
    if (_internal_rotation & 1)
    {
      std::swap(pw, ph);
      std::swap(mw, mh);
    }
    _width  = pw;
    _height = ph;

    _xs = _xe = _ys = _ye = INT16_MAX;
    _xs_raw = _xe_raw = _ys_raw = _ye_raw = INT16_MAX;

//    _update_colmod();
  }

  void Panel_M5UnitLCD::_update_colmod(void)
  {
/*
    if (_bus == nullptr) return;

    startWrite();
    _bus->writeCommand(CMD_COLMOD | (_write_bits | _internal_rotation) << 8, 16);
    _bus->writeCommand(CMD_COLMOD | _write_bits << 8, 16);
    endWrite();
*/
  }

  void Panel_M5UnitLCD::setInvert(bool invert)
  {
    _invert = invert;
    startWrite();
    _bus->writeCommand((invert ^ _cfg.invert) ? CMD_INVON : CMD_INVOFF, 8);
    endWrite();
  }

  void Panel_M5UnitLCD::setBrightness(std::uint8_t brightness)
  {
    startWrite();
    _bus->writeCommand(CMD_BRIGHTNESS | brightness << 8, 16);
    endWrite();
  }


  void Panel_M5UnitLCD::writeBlock(std::uint32_t rawcolor, std::uint32_t length)
  {
    do
    {
      std::uint32_t h = 1;
      auto w = std::min<std::uint32_t>(length, _xe + 1 - _xpos);
      if (length >= (w << 1) && _xpos == _xs)
      {
        h = std::min<std::uint32_t>(length / w, _ye + 1 - _ypos);
      }
      writeFillRectPreclipped(_xpos, _ypos, w, h, rawcolor);
      if ((_xpos += w) <= _xe) return;
      _xpos = _xs;
      if (_ye < (_ypos += h)) { _ypos = _ys; }
      length -= w * h;
    } while (length);
/*
    std::size_t bytes = (rawcolor == 0) ? 1 : (_write_bits >> 3);
    std::uint8_t buf[(length >> 8) * (bytes + 1) + 2];
    buf[0] = CMD_WR_RLE | bytes;
    std::size_t idx = 1;
    do
    {
      std::uint32_t len = (length < 0x100)
                        ? length : 0xFF;
      buf[idx++] = len;
      auto color = rawcolor;
      for (int i = bytes; i > 0; --i)
      {
        buf[idx++] = color;
        color >>= 8;
      }
      length -= len;
    } while (length);
    _bus->writeBytes(buf, idx, false, true);
*/
  }

  void Panel_M5UnitLCD::drawPixelPreclipped(std::uint_fast16_t x, std::uint_fast16_t y, std::uint32_t rawcolor)
  {
    startWrite();
    writeFillRectPreclipped(x, y, 1, 1, rawcolor);
    endWrite();
  }

  void Panel_M5UnitLCD::writeFillRectPreclipped(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, std::uint32_t rawcolor)
  {
    if (_internal_rotation)
    {
      auto r = _internal_rotation;
      if (r & 4)     { y = _height - (y + h); }
      if (r & 1)     { std::swap(x, y);  std::swap(w, h); }
      if ((r+1) & 2) { x = _cfg.panel_width  - (x + w); }
      if (r     & 2) { y = _cfg.panel_height - (y + h); }
    }
    auto xs = x;
    auto ys = y;
    auto xe = x + w - 1;
    auto ye = y + h - 1;
    std::uint8_t buf[16];
    std::size_t idx = 0;
    if (xs != _xs_raw || xe != _xe_raw)
    {
      _xs_raw = xs;
      _xe_raw = xe;
      buf[idx++] = CMD_CASET;
      if (_cfg.memory_width >= 256) buf[idx++] = xs >> 8;
      buf[idx++] = xs;
      if (_cfg.memory_width >= 256) buf[idx++] = xe >> 8;
      buf[idx++] = xe;
    }
    if (ys != _ys_raw || ye != _ye_raw)
    {
      _ys_raw = ys;
      _ye_raw = ye;
      buf[idx++] = CMD_RASET;
      if (_cfg.memory_height >= 256) buf[idx++] = ys >> 8;
      buf[idx++] = ys;
      if (_cfg.memory_height >= 256) buf[idx++] = ye >> 8;
      buf[idx++] = ye;
    }
    if (_raw_color != rawcolor)
    {
      _raw_color = rawcolor;
      std::size_t bytes = (rawcolor == 0) ? 1 : (_write_bits >> 3);
      buf[idx++] = CMD_SET_COLOR | bytes;
      for (int i = bytes; i > 0; --i)
      {
        buf[idx++] = rawcolor;
        rawcolor >>= 8;
      }
    }
    buf[idx++] = CMD_RAM_FILL;
    _bus->writeBytes(buf, idx, false, true);
  }

  void Panel_M5UnitLCD::setWindow(std::uint_fast16_t xs, std::uint_fast16_t ys, std::uint_fast16_t xe, std::uint_fast16_t ye)
  {
    _xpos = xs;
    _xs = xs;
    _xe = xe;
    _ypos = ys;
    _ys = ys;
    _ye = ye;
/*
    auto r = _internal_rotation;
    if (r)
    {
      if (r & 1) { std::swap(xs, ys); std::swap(xe, ye); }
      if ((1 + r - (r >> 2)) & 2)  // case 1:2:6:7:
      {
        std::swap(xs, xe);
        xs = _cfg.panel_width - 1 - xs;
        xe = _cfg.panel_width - 1 - xe;
      }

      if ((r - (r >> 2)) & 2) // case 2:3:4:7:
      {
        std::swap(ys, ye);
        ys = _cfg.panel_height - 1 - ys;
        ye = _cfg.panel_height - 1 - ye;
      }
    }
    _set_window(xs, ys, xe, ye);
*/
  }
  void Panel_M5UnitLCD::_set_window(std::uint_fast16_t xs, std::uint_fast16_t ys, std::uint_fast16_t xe, std::uint_fast16_t ye)
  {
    std::uint8_t buf[7];
    std::size_t idx = 0;
    if (xs != _xs_raw || xe != _xe_raw)
    {
      _xs_raw = xs;
      _xe_raw = xe;
      buf[idx  ] = CMD_CASET;
      buf[idx+1] = xs;
      buf[idx+2] = xe;
      idx += 3;
    }
    if (ys != _ys_raw || ye != _ye_raw)
    {
      _ys_raw = ys;
      _ye_raw = ye;
      buf[idx  ] = CMD_RASET;
      buf[idx+1] = ys;
      buf[idx+2] = ye;
      idx += 3;
    }
//  buf[idx++] = CMD_RAMWR;
    _bus->writeBytes(buf, idx, false, true);
  }



  static std::uint8_t* store_encoded(
    std::uint8_t* dest,
    const std::uint8_t* src,
    std::size_t data_num,
    std::size_t bytes)
  {
    if (data_num >= 1)
    {
      *dest++ = data_num;
      memmove(dest, src, bytes);
      dest += bytes;
    }
    return dest;
  }

  static std::uint8_t* store_absolute(
    std::uint8_t* dest,
    const std::uint8_t* src,
    std::size_t src_size,
    std::size_t bytes)
  {
    const std::uint8_t* psrc = src;
 
    if (src_size >= 3)  // 絶対モード
    {
      *dest++ = 0x00;
      *dest++ = src_size;
      memmove(dest, psrc, src_size * bytes);
      dest += src_size * bytes;
    }
    else  // RLEモード
    {
      for (long i = 0; i < src_size; i++)
      {
        dest = store_encoded(dest, psrc + i * bytes, 1, bytes);
      }
    }
 
    return dest;
  }

  static std::size_t rleEncode(std::uint8_t* dest, const std::uint8_t* src, std::size_t bytelen, std::size_t bytes)
  {
    // 行データのバイト幅及び最大連続バイト数設定
    long maxbyte = 255;

    // ポインタ設定
    std::uint8_t* pdest = dest;
 
    // 絶対モード開始位置ポインタ
    const std::uint8_t* pabs = src;
 
    // 前回バイト位置保持変数
    const std::uint8_t* prev = src;
 
    // 連続回数及び絶対モードカウンタ
    long cont = 1;
    long abso = -1;
 
    // データ分だけループ
    for (long i = bytes; i < bytelen; i += bytes)
    {
        // 現在位置のバイト値取得
        std::size_t byteidx = 0;
        while (src[i + byteidx] == prev[byteidx] && ++byteidx != bytes);
 
        // 前のバイト値と等しいかどうかで処理分け
        if (byteidx == bytes)
        {
            // 連続数カウントアップ
            cont++;
 
            if (abso >= 1)
            {
                // 今までの不連続値があれば書き出し
                pdest = store_absolute(pdest, pabs, abso, bytes);
            }
            else if (cont == maxbyte)
            {
                // 最大バイト数になったならエンコードデータ書き出し
                pdest = store_encoded(pdest, prev, maxbyte, bytes);
                cont = 1;
 
                prev = src + i;
            }
 
            // 不連続数初期化
            abso = -1;
        }
        else
        {
            // 不連続数カウントアップ
            // prev までの不連続数(cur は次のバイト値と等しい場合がある為)
            abso++;
 
            if (cont >= 2)
            {
                // 今までの連続データがあればエンコードデータ書き出し
                pdest = store_encoded(pdest, prev, cont, bytes);
            }
            else if (abso == maxbyte)
            {
                // 最大バイト数になったなら絶対モードデータ書き出し
                pdest = store_absolute(pdest, pabs, maxbyte, bytes);
                abso = 0;
            }
 
            // 連続数初期化(cur を含む)
            cont = 1;
 
            // 絶対モード開始位置移動
            if (abso == 0) { pabs = src + i; }
 
            // 前のバイト値置き換え
            prev = src + i;
        }
    }
 
    // 残りのデータを書き出し
    if (abso >= 0)
    {
        pdest = store_absolute(pdest, pabs, abso + 1, bytes);
    }
    else if (cont >= 2)
    {
        pdest = store_encoded(pdest, prev, cont, bytes);
    }
 
    return pdest - dest;
  }

  void Panel_M5UnitLCD::writeImage(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    if (_internal_rotation)
    {
      auto r = _internal_rotation;
      if (r & 4)     { y = _height - (y + h); }
      if (r & 1)     { std::swap(x, y);  std::swap(w, h); }
      if ((r+1) & 2) { x = _cfg.panel_width  - (x + w); }
      if (r     & 2) { y = _cfg.panel_height - (y + h); }
    }

    _set_window(x, y, x+w-1, y+h-1);

    auto bytes = param->dst_bits >> 3;
    auto src_x = param->src_x;

    h += y;
    std::uint32_t wb = w * bytes;
//*
    auto dmabuf = _bus->getDMABuffer(wb+1);
    dmabuf[0] = CMD_WR_RAW | _write_bits>>3;
    do
    {
      std::uint32_t i = 0;
      while (w != (i = param->fp_skip(i, w, param)))
      {
        auto buf = &dmabuf[1];
        std::int32_t len = param->fp_copy(buf, 0, w - i, param);
        //setWindow(x + i, y, x + i + len - 1, y);
        _bus->writeBytes(dmabuf, 1+len * bytes, false, true);
        if (w == (i += len)) break;
      }
      param->src_x = src_x;
      param->src_y++;
    } while (++y != h);
/*/
    auto dmabuf = _bus->getDMABuffer(wb + (wb >> 8) * 2 + 6);
    dmabuf[0] = CMD_WR_RLE | _write_bits >> 3;
    do
    {
      std::uint32_t i = 0;
      while (w != (i = param->fp_skip(i, w, param)))
      {
        auto buf = &dmabuf[(wb >> 8) * 2 + 4];
        std::int32_t len = param->fp_copy(buf, 0, w - i, param);
        //setWindow(x + i, y, x + i + len - 1, y);
Serial.printf("writeImage len:%d bytes:%d  ", len, bytes);
        len = rleEncode(&dmabuf[1], buf, len * bytes, bytes);
Serial.printf("rle len:%d \r\n", len);
delay(10);
        _bus->writeBytes(dmabuf, 1 + len, false, true);
        if (w == (i += len)) break;
      }
      param->src_x = src_x;
      param->src_y++;
    } while (++y != h);
//*/
  }

//----------------------------------------------------------------------------
 }
}
