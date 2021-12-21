#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>

# define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/printer/Printer_EM5820.hpp>
#include <lgfx/v1/platforms/arduino_default/Bus_Stream.hpp>

class LGFX_AtomPrinter : public lgfx::LGFX_Device
{
  lgfx::Printer_EM5820      _panel_instance;
  lgfx::Bus_Stream          _bus_instance;

public:
  LGFX_AtomPrinter(Stream* stream, uint16_t height, int rts = -1)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.stream = stream;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_busy = rts;
      cfg.memory_height = cfg.panel_height = height;
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);
  }
};

static LGFX_AtomPrinter printer(&Serial1, 192, 19);

void setup() {
  M5.begin(true, false, true);
  Serial1.begin(9600, SERIAL_8N1, 33, 23);

  Serial.println("WiFi");
#if defined (wifi_ssid) && defined (wifi_pass)
  WiFi.begin(wifi_ssid, wifi_pass);
#else
  WiFi.begin();
#endif  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  
  printer.init();
  
  printer.drawPngUrl("http://t.wakwak-koba.jp/garakuta/monodora.png");

  printer.setTextColor(TFT_BLACK);
  printer.setFont(&fonts::Font2);
  printer.println("HELLO DORAEMON");

  printer.display();
}

void loop() {
}
