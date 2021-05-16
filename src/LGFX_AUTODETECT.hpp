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

#ifndef LGFX_USE_V1
#define LGFX_USE_V1
#endif

#include "LovyanGFX.hpp"

namespace lgfx
{
  namespace boards
  {
    enum board_t
    { board_unknown
    , board_Non_Panel
    , board_M5Stack
    , board_M5StackCore2
    , board_M5StickC
    , board_M5StickCPlus
    , board_TTGO_TS
    , board_TTGO_TWatch
    , board_TTGO_TWristband
    , board_ODROID_GO
    , board_DDUINO32_XS
    , board_ESP_WROVER_KIT
    , board_LoLinD32
    , board_WioTerminal
    , board_WiFiBoy_Pro
    , board_WiFiBoy_Mini
    , board_Makerfabs_TouchCamera
    , board_Makerfabs_MakePython
    , board_M5StackCoreInk
    , board_M5Stack_CoreInk = board_M5StackCoreInk
    , board_M5Paper
    , board_ESP32_S2_Kaluga_1
    };
  }
  using namespace boards;
}


#if defined (ESP32) || defined (CONFIG_IDF_TARGET_ESP32) || defined (ESP_PLATFORM)

 #if defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE )
   #define LGFX_M5STACK
 #elif defined( ARDUINO_M5STACK_Core2 ) // M5Stack Core2
   #define LGFX_M5STACK_CORE2
 #elif defined( ARDUINO_M5Stick_C ) // M5Stick C / CPlus
   #define LGFX_M5STICK_C
 #elif defined( ARDUINO_M5Stick_C_Plus )
   #define LGFX_M5STICK_C
 #elif defined( ARDUINO_M5Stack_CoreInk ) // M5Stack CoreInk
   #define LGFX_M5STACK_COREINK
 #elif defined( ARDUINO_M5STACK_Paper ) // M5Paper
   #define LGFX_M5PAPER
 #elif defined( ARDUINO_ODROID_ESP32 ) // ODROID-GO
   #define LGFX_ODROID_GO
 #elif defined( ARDUINO_TTGO_T1 ) // TTGO TS
   #define LGFX_TTGO_TS
 #elif defined( ARDUINO_TWatch ) || defined( ARDUINO_T ) // TTGO T-Watch
   #define LGFX_TTGO_TWATCH
 #elif defined( ARDUINO_D ) || defined( ARDUINO_DDUINO32_XS ) // DSTIKE D-duino-32 XS
   #define LGFX_DDUINO32_XS
 #elif defined( ARDUINO_LOLIN_D32_PRO )
   #define LGFX_LOLIN_D32_PRO
 #elif defined( ARDUINO_ESP32_WROVER_KIT )
   #define LGFX_ESP_WROVER_KIT
 #endif

 #include "lgfx/v1_autodetect/LGFX_AutoDetect_ESP32.hpp"

#elif defined (__SAMD51__)

 #if defined( LGFX_WIO_TERMINAL ) || defined (ARDUINO_WIO_TERMINAL) || defined(WIO_TERMINAL)

  #include "lgfx/v1_autodetect/LGFX_AutoDetect_SAMD51.hpp"

 #endif

#else

  #include "lgfx/v1_autodetect/LGFX_AutoDetect_STM32.hpp"

#endif

