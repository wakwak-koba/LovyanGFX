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

#include "../Panel.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------
  struct IBus;
  struct ILight;

  class Panel_Device : public IPanel
  {
  public:
    Panel_Device(void);

    void bus(IBus* bus);
    inline IBus* bus(void) const { return _bus; }

    inline void setLight(ILight* light) { _light = light; }
    inline ILight* getLight(void) const { return _light; }

    struct config_t
    {
      /// Number of CS pin
      /// CS ピン番号
      std::int16_t pin_cs = -1;

      /// Number of RST pin
      /// RST ピン番号
      std::int16_t pin_rst = -1;

      /// Number of BUSY pin
      /// BUSY ピン番号
      std::int16_t pin_busy = -1;

      /// The maximum width of an image that the LCD driver can handle.
      /// LCDドライバが扱える画像の最大幅
      std::uint16_t memory_width = 240;

      /// The maximum height of an image that the LCD driver can handle.
      /// LCDドライバが扱える画像の最大高さ
      std::uint16_t memory_height = 240;

      /// Actual width of the display.
      /// 実際に表示できる幅
      std::uint16_t panel_width = 240;

      /// Actual height of the display.
      /// 実際に表示できる高さ
      std::uint16_t panel_height = 240;

      /// Number of offset pixels in the X direction.
      /// パネルのX方向オフセット量
      std::uint16_t offset_x = 0;

      /// Number of offset pixels in the Y direction.
      /// パネルのY方向オフセット量
      std::uint16_t offset_y = 0;

      /// Offset value in the direction of rotation. 0~7 (4~7 is upside down)
      /// 回転方向のオフセット 0~7 (4~7は上下反転)
      std::uint8_t offset_rotation = 0;

      /// Number of bits in dummy read before pixel readout.
      /// ピクセル読出し前のダミーリードのビット数
      std::uint8_t dummy_read_pixel = 8;

      /// Number of bits in dummy read before data readout.
      /// データ読出し前のダミーリードのビット数
      std::uint8_t dummy_read_bits = 1;

      /// Whether the data is readable or not.
      /// データ読出しが可能か否か
      bool readable = true;

      /// brightness inversion (e.g. IPS panel)
      /// 明暗の反転 (IPSパネルはtrueに設定)
      bool invert = false;

      /// Set the RGB/BGR color order.
      /// RGB=true / BGR=false パネルの赤と青が入れ替わってしまう場合 trueに設定
      bool rgb_order = false;

      /// 16-bit alignment of transmitted data
      /// 送信データの16bitアライメント データ長を16bit単位で送信するパネルの場合 trueに設定
      bool dlen_16bit = false;

      /// Whether or not to share the bus with the file system (if set to true, drawJpgFile etc. will control the bus)
      /// SD等のファイルシステムとのバス共有の有無 (trueに設定するとdrawJpgFile等でバス制御が行われる)
      bool bus_shared = true;
    };

    const config_t& config(void) const { return _cfg; }
    void config(const config_t& cfg) { _cfg = cfg; }


    bool isReadable(void) const override { return _cfg.readable; }
    bool isBusShared(void) const override { return _cfg.bus_shared; }

    void setBrightness(std::uint8_t brightness) override;

    void init(bool use_reset) override;
    void initDMA(void) override;
    void waitDMA(void) override;
    bool dmaBusy(void) override;

    void writeCommand(std::uint32_t data, std::uint_fast8_t length) override;
    void writeData(std::uint32_t data, std::uint_fast8_t length) override;
    void writeImageARGB(std::uint_fast16_t x, std::uint_fast16_t y, std::uint_fast16_t w, std::uint_fast16_t h, pixelcopy_t* param) override;
    void copyRect(std::uint_fast16_t dst_x, std::uint_fast16_t dst_y, std::uint_fast16_t w, std::uint_fast16_t h, std::uint_fast16_t src_x, std::uint_fast16_t src_y) override;

  protected:

    static constexpr std::uint8_t CMD_INIT_DELAY = 0x80;

    config_t _cfg;

    IBus* _bus = nullptr;
    ILight* _light = nullptr;
    bool _align_data = false;
    std::uint8_t _internal_rotation = 0;

    /// Performs preparation processing for the CS pin.
    /// If you want to control the CS pin on your own, override this function and implement it.
    /// CSピンの準備処理を行う。CSピンを自前で制御する場合、この関数をoverrideして実装すること。
    virtual void init_cs(void);

    /// Controls the CS pin to go HIGH when the argument is true.
    /// If you want to control the CS pin on your own, override this function and implement it.
    /// 引数に応じてCSピンを制御する。false=LOW / true=HIGH。CSピンを自前で制御する場合、この関数をoverrideして実装すること。
    virtual void cs_control(bool level);

    /// Performs preparation processing for the RST pin.
    /// If you want to control the RST pin on your own, override this function and implement it.
    /// RSTピンの準備処理を行う。RSTピンを自前で制御する場合、この関数をoverrideして実装すること。
    virtual void init_rst(void);

    /// Bring the RST pin low once and bring it back high.
    /// If you want to control the RST pin on your own, override this function and implement it.
    /// RSTピンを一度LOWにし、HIGHに戻す。RSTピンを自前で制御する場合、この関数をoverrideして実装すること。
    virtual void reset(void);

    /// Get the panel initialization command sequence.
    /// パネルの初期化コマンド列を得る。無い場合はnullptrを返す。
    virtual const std::uint8_t* getInitCommands(std::uint8_t listno) const { (void)listno; return nullptr; }

    enum fastread_dir_t
    {
      fastread_nothing,
      fastread_horizontal,
      fastread_vertical,
    };
    virtual fastread_dir_t get_fastread_dir(void) const { return fastread_nothing; }

    void command_list(const std::uint8_t *addr);

  };

//----------------------------------------------------------------------------
 }
}
