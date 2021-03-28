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
#if defined (ESP32) || defined (CONFIG_IDF_TARGET_ESP32) || defined (CONFIG_IDF_TARGET_ESP32S2) || defined (ESP_PLATFORM)

#include "Bus_I2C.hpp"
#include "../../misc/pixelcopy.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void Bus_I2C::config(const config_t& config)
  {
    _cfg = config;

  }

  void Bus_I2C::init(void)
  {
    lgfx::i2c::init(_cfg.i2c_port, _cfg.pin_sda, _cfg.pin_scl, _cfg.freq);
  }

  void Bus_I2C::release(void)
  {
  }

  void Bus_I2C::beginTransaction(void)
  {
    lgfx::i2c::setClock(_cfg.i2c_port, _cfg.freq);
  }

  void Bus_I2C::endTransaction(void)
  {
  }

  void Bus_I2C::beginRead(void)
  {
  }

  void Bus_I2C::endRead(void)
  {
  }

  void Bus_I2C::wait(void)
  {
  }

  bool Bus_I2C::busy(void) const
  {
    return false;
  }

  void Bus_I2C::writeCommand(std::uint32_t data, std::uint_fast8_t bit_length)
  {
    std::uint8_t buf[8];
    std::size_t idx = 0;
    for (; idx < _cfg.prefix_len; ++idx)
    {
      buf[idx] = _cfg.prefix_cmd >> (idx << 3);
    }
    memcpy(&buf[idx], &data, 4);
    //*reinterpret_cast<std::uint32_t*>(&buf[1]) = data;
    lgfx::i2c::writeBytes(_cfg.i2c_port, _cfg.i2c_addr, buf, (bit_length >> 3) + idx);
  }

  void Bus_I2C::writeData(std::uint32_t data, std::uint_fast8_t bit_length)
  {
    std::uint8_t buf[8];
    std::size_t idx = 0;
    for (; idx < _cfg.prefix_len; ++idx)
    {
      buf[idx] = _cfg.prefix_data >> (idx << 3);
    }
    memcpy(&buf[idx], &data, 4);
    //*reinterpret_cast<std::uint32_t*>(&buf[1]) = data;
    lgfx::i2c::writeBytes(_cfg.i2c_port, _cfg.i2c_addr, buf, (bit_length >> 3) + idx);
  }

  void Bus_I2C::writeDataRepeat(std::uint32_t data, std::uint_fast8_t bit_length, std::uint32_t length)
  {
    const std::uint8_t dst_bytes = bit_length >> 3;
    std::uint32_t limit = (dst_bytes == 3) ? 12 : 16;
    auto buf = _flip_buffer.getBuffer(512);
    std::size_t fillpos = 0;
    for (; fillpos < _cfg.prefix_len; ++fillpos)
    {
      buf[fillpos] = _cfg.prefix_data >> (fillpos << 3);
    }
    auto dmabuf = &buf[fillpos];
    memcpy(&buf[fillpos], &data, 4);
    fillpos += dst_bytes;
    std::uint32_t len;
    do
    {
      len = ((length - 1) % limit) + 1;
      if (limit <= 16) limit <<= 1;

      while (fillpos < len * dst_bytes)
      {
        memcpy(&dmabuf[fillpos], dmabuf, std::min(fillpos, len * dst_bytes - fillpos));
        fillpos += fillpos;
      }
      i2c::writeBytes(_cfg.i2c_port, _cfg.i2c_addr, buf, len * dst_bytes + 1);
    } while (length -= len);
  }

  void Bus_I2C::writePixels(pixelcopy_t* param, std::uint32_t length)
  {
    const std::uint8_t dst_bytes = param->dst_bits >> 3;
    std::uint32_t limit = (dst_bytes == 3) ? 12 : 16;
    std::uint32_t len;
    do
    {
      len = ((length - 1) % limit) + 1;
      if (limit <= 64) limit <<= 1;
      auto dmabuf = _flip_buffer.getBuffer(len * dst_bytes);
      param->fp_copy(dmabuf, 0, len, param);
      writeBytes(dmabuf, len * dst_bytes, true, true);
    } while (length -= len);
  }

  void Bus_I2C::writeBytes(const std::uint8_t* data, std::uint32_t length, bool dc, bool use_dma)
  {
    if (_cfg.prefix_len)
    {
      auto buf = _flip_buffer.getBuffer(length + _cfg.prefix_len);
      memcpy(buf, dc ? &_cfg.prefix_data : &_cfg.prefix_cmd, 4);
      memcpy(&buf[_cfg.prefix_len], data, length);
      i2c::writeBytes(_cfg.i2c_port, _cfg.i2c_addr, buf, length + _cfg.prefix_len);
    }
    else
    {
      i2c::writeBytes(_cfg.i2c_port, _cfg.i2c_addr, data, length);
    }
  }

  std::uint32_t Bus_I2C::readData(std::uint_fast8_t bit_length)
  {
    return 0;
  }

  void Bus_I2C::readBytes(std::uint8_t* dst, std::uint32_t length, bool use_dma)
  {
  }

  void Bus_I2C::readPixels(void* dst, pixelcopy_t* param, std::uint32_t length)
  {
  }

//----------------------------------------------------------------------------
 }
}

#endif
