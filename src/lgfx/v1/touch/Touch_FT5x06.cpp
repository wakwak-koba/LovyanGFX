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
#include "Touch_FT5x06.hpp"

#include "../platforms/common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr std::uint8_t FT5x06_VENDID_REG = 0xA8;
  static constexpr std::uint8_t FT5x06_POWER_REG  = 0x87;
  static constexpr std::uint8_t FT5x06_INTMODE_REG= 0xA4;

  static constexpr std::uint8_t FT5x06_MONITOR  = 0x01;
  static constexpr std::uint8_t FT5x06_SLEEP_IN = 0x03;

  bool Touch_FT5x06::_write_reg(std::uint8_t reg, std::uint8_t val)
  {
    lgfx::i2c::beginTransaction(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq);
    std::uint8_t data[] = { reg, val };
    return lgfx::i2c::writeBytes(_cfg.i2c_port, data, 2)
        && lgfx::i2c::endTransaction(_cfg.i2c_port);
  }

  bool Touch_FT5x06::_read_reg(std::uint8_t reg, std::uint8_t *data, std::size_t length)
  {
    return lgfx::i2c::readRegister(_cfg.i2c_port, _cfg.i2c_addr, reg, data, length, _cfg.freq);
  }

  bool Touch_FT5x06::_check_init(void)
  {
    if (_inited) return true;

    std::uint8_t tmp[2] = { 0 };
    _inited = _write_reg(0x00, 0x00)
          && _read_reg(FT5x06_VENDID_REG, tmp, 1)
          && tmp[0]
          ;

    return _inited;
  }

  bool Touch_FT5x06::init(void)
  {
    _inited = false;
    if (isSPI()) return false;

    lgfx::i2c::init(_cfg.i2c_port, _cfg.pin_sda, _cfg.pin_scl);
    if (_cfg.pin_int >= 0)
    {
      lgfx::pinMode(_cfg.pin_int, pin_mode_t::input_pullup);
    }
    return true;
  }

  void Touch_FT5x06::wakeup(void)
  {
    if (!_check_init()) return;
    _write_reg(FT5x06_POWER_REG, FT5x06_MONITOR);
  }

  void Touch_FT5x06::sleep(void)
  {
    if (!_check_init()) return;
    _write_reg(FT5x06_POWER_REG, FT5x06_SLEEP_IN);
  }

  std::uint_fast8_t Touch_FT5x06::getTouchRaw(touch_point_t *tp, std::uint_fast8_t number)
  {
    if (tp) tp->size = 0;
    if (!_check_init() || number > 4) return 0;
    if (_cfg.pin_int >= 0)
    {
      if (_flg_released != (bool)gpio_in(_cfg.pin_int))
      {
        _flg_released = !_flg_released;
        _write_reg(FT5x06_INTMODE_REG, 0x00); // INT Polling mode
      }
      if (_flg_released)
      {
        return 0;
      }
    }

    std::uint_fast16_t tx, ty;
    std::int32_t retry = 3;
    std::uint32_t base = number * 6;
    std::uint8_t tmp[base + 5];
    do
    {
      _read_reg(0x02, tmp, 5 + base);
      if (number >= tmp[0]) return 0;
      tx = (tmp[base + 1] & 0x0F) << 8 | tmp[base + 2];
      ty = (tmp[base + 3] & 0x0F) << 8 | tmp[base + 4];
    } while ((tx > _cfg.x_max || ty > _cfg.y_max) && --retry);

    if (tp)
    {
      tp->x = tx;
      tp->y = ty;
      tp->id = tmp[base + 3] >> 4;
      tp->size = 1;
    }
    return tmp[0];
  }

//----------------------------------------------------------------------------
 }
}
