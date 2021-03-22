#pragma once

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Bus_Parallel8 _bus_instance;    // Instance of connection bus. 通信バスのインスタンス
  lgfx::Panel_ILI9342 _panel_instance;  // Instance of display panel.  表示パネルのインスタンス

public:

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();    // Get config params of bus.  バスの設定値を取得

// For Bus_Parallel (ESP32)
      cfg.i2s_port = i2s_port_t::I2S_NUM_0; // Set the port of I2S  (I2S_NUM_0 or I2S_NUM_1)  使用するI2Sポートを選択
      cfg.freq_write = 20000000;
      cfg.pin_wr =  4;
      cfg.pin_rd =  2;
      cfg.pin_rs = 15;
      cfg.pin_d0 = 12;
      cfg.pin_d1 = 13;
      cfg.pin_d2 = 26;
      cfg.pin_d3 = 25;
      cfg.pin_d4 = 17;
      cfg.pin_d5 = 16;
      cfg.pin_d6 = 27;
      cfg.pin_d7 = 14;
      _bus_instance.config(cfg);    // Set config params of bus.  バスの設定値を更新
      _panel_instance.bus(&_bus_instance);  // Set bus to panel. バスをパネルにセット
    }

    {
      auto cfg = _panel_instance.config();  // Set config params of panel.  パネルの設定値を取得

      cfg.pin_cs          =    33;  /// CSが接続されているピン番号
      cfg.pin_rst         =    32;  /// RSTが接続されているピン番号
      cfg.pin_busy        =    -1;  /// BUSYが接続されているピン番号
      cfg.memory_width    =   320;  /// ドライバがサポートしている最大の幅
      cfg.memory_height   =   240;  /// ドライバがサポートしている最大の高さ
      cfg.panel_width     =   320;  /// 実際に表示可能な幅
      cfg.panel_height    =    96;  /// 実際に表示可能な高さ
      cfg.offset_x        =     0;  /// パネルのX方向オフセット量
      cfg.offset_y        =   144;  /// パネルのY方向オフセット量
      cfg.offset_rotation =     0;  /// 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.readable        =  true;  /// データ読出しが可能な場合 trueに設定
      cfg.invert          = false;  /// パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order       = false;  /// パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit      = false;  /// データ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared      = false;  /// SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);             // Set the display panel to be used. 使用するパネルをセット

/*
    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = 32;     /// バックライトが接続されているピン番号
      cfg.invert = false;  /// バックライトの輝度を反転させる場合 true
      cfg.freq   = 44100;  /// バックライトのPWM周波数
      cfg.pwm_channel = 7; /// 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
    }
*/

  }
};

