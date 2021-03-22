#pragma once

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
// Change the instance type according to your configuration
// 構成に応じてインスタンスの型を変更してください
  lgfx::Bus_SPI       _bus_instance;    // Instance of connection bus. 通信バスのインスタンス
  lgfx::Panel_ILI9342 _panel_instance;  // Instance of display panel.  表示パネルのインスタンス
  lgfx::Light_PWM     _light_instance;  // Instance of backlight.      バックライトのインスタンス(必要なければ省略)
//lgfx::Touch_FT5x06  _touch_instance;  // Instance of touchscreen.    タッチパネルのインスタンス(必要なければ省略)
/*
  lgfx::Bus_SPI       _bus_instance;    // for SPI
  lgfx::Bus_Parallel  _bus_instance;    // for Parallel 8bit (only ESP32)

  lgfx::Panel_ILI9342  _panel_instance;
  lgfx::Panel_HX8357B  _panel_instance;
  lgfx::Panel_HX8357D  _panel_instance;
  lgfx::Panel_ILI9163  _panel_instance;
  lgfx::Panel_ILI9341  _panel_instance;
  lgfx::Panel_ILI9342  _panel_instance;
  lgfx::Panel_ILI9486  _panel_instance;
  lgfx::Panel_ILI9486L _panel_instance;
  lgfx::Panel_ILI9488  _panel_instance;
  lgfx::Panel_SSD1351  _panel_instance;
  lgfx::Panel_ST7735   _panel_instance;
  lgfx::Panel_ST7735S  _panel_instance;
  lgfx::Panel_ST7789   _panel_instance;
  lgfx::Panel_ST7796   _panel_instance;

  lgfx::Touch_SPI_STMPE610 _touch_instance;
  lgfx::Touch_FT5x06       _touch_instance;
  lgfx::Touch_XPT2046      _touch_instance;
*/

public:

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();    // Get config params of bus.  バスの設定値を取得

// For Bus_SPI (ESP32)
      cfg.spi_host = spi_host_device_t::VSPI_HOST; // Set the type of SPI  (VSPI_HOST or HSPI_HOST)  使用するSPIを選択
      cfg.spi_mode = 0;             // Set the SPI mode. (0 ~ 3)      SPI通信モードを設定
      cfg.freq_write = 40000000;    // Set the SPI clock for write.   送信時のSPIクロックを設定
      cfg.freq_read  = 16000000;    // Set the SPI clock for read.    受信時のSPIクロックを設定
      cfg.spi_3wire  = true;        // Set to "true" for a panel that uses MOSI pins to read data.  受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock   = true;        // Set to "true" if transaction locking is to be used.  トランザクションロックを使用する場合はtrueを設定

      cfg.dma_channel = 1;          // Set the DMA channel (1 or 2. 0=disable)   使用するDMAチャンネルを設定 (0=DMA不使用)
      cfg.pin_sclk = 18;            // Set the SPI SCLK pin number   SPIのSCLKピン番号を設定
      cfg.pin_mosi = 23;            // Set the SPI MOSI pin number   SPIのMOSIピン番号を設定
      cfg.pin_miso = 19;            // Set the SPI MISO pin number   SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc   = 27;            // Set the SPI D/C pin number    SPIのD/Cピン番号を設定 (-1 = disable)
     // ※ If you share the SPI bus with an SD card, be sure to set up MISO as well.
     //    SDカードと共通のSPIバスを使う場合、MISOは省略せず必ず設定してください。

      _bus_instance.config(cfg);    // Set config params of bus.  バスの設定値を更新
      _panel_instance.bus(&_bus_instance);  // Set bus to panel. バスをパネルにセット
    }

    {
      auto cfg = _panel_instance.config();  // Set config params of panel.  パネルの設定値を取得

      cfg.pin_cs           =    14;  /// CSが接続されているピン番号
      cfg.pin_rst          =    33;  /// RSTが接続されているピン番号
      cfg.pin_busy         =    -1;  /// BUSYが接続されているピン番号
      cfg.memory_width     =   320;  /// ドライバがサポートしている最大の幅
      cfg.memory_height    =   240;  /// ドライバがサポートしている最大の高さ
      cfg.panel_width      =   320;  /// 実際に表示可能な幅
      cfg.panel_height     =   240;  /// 実際に表示可能な高さ
      cfg.offset_x         =     0;  /// パネルのX方向オフセット量
      cfg.offset_y         =     0;  /// パネルのY方向オフセット量
      cfg.offset_rotation  =     3;  /// 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel =     8;  /// ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits  =     1;  /// ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable         =  true;  /// データ読出しが可能な場合 trueに設定
      cfg.invert           =  true;  /// パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order        = false;  /// パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit       = false;  /// データ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared       =  true;  /// SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }
    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = 32;     /// バックライトが接続されているピン番号
      cfg.invert = false;  /// バックライトの輝度を反転させる場合 true
      cfg.freq   = 44100;  /// バックライトのPWM周波数
      cfg.pwm_channel = 7; /// 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
    }
/*
    {
      auto cfg = _touch_instance.config();


      _touch_instance.config(cfg);
    }
//*/

    setPanel(&_panel_instance);             // Set the display panel to be used. 使用するパネルをセット
    setLight(&_light_instance);             // Set the backlight to be used. 使用するバックライトをセット
//  setTouch(&_touch_instance);             // Set the touchscreen to be used. 使用するタッチパネルをセット


  }
};

