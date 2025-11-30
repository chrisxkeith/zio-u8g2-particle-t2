// Testbed for determining why U8g2lib with the ZIO OLED display crashes

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

void publishEvent(const char* eventName, const char* data) {
    Particle.publish(eventName, data, PRIVATE);
    delay(4000); // Allow time for the publish to complete
}

#include <U8g2lib.h>

U8G2_SSD1327_EA_W128128_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); /* Uno: A4=SDA, A5=SCL, add "u8g2.setBusClock(400000);" into setup() for speedup if possible */

const int COLOR_WHITE = 1;
const int COLOR_BLACK = 0;
class OLEDWrapper {
  public:
    void u8g2_prepare(void) {
      u8g2.setFont(u8g2_font_fur49_tn);
      u8g2.setFontRefHeightExtendedText();
      u8g2.setDrawColor(COLOR_WHITE);
      u8g2.setFontDirection(0);
    }
    void startup() {
      pinMode(10, OUTPUT);
      pinMode(9, OUTPUT);
      digitalWrite(10, 0);
      digitalWrite(9, 0);
      publishEvent("startup()", "before u8g2.initDisplay()");
      u8g2.initDisplay();
//      publishEvent("startup()", "before u8g2.begin()");
//      u8g2.begin();
/*      publishEvent("startup()", "after u8g2.begin()");
      publishEvent("startup()", "before u8g2.setBusClock()");
      u8g2.setBusClock(400000);
      publishEvent("startup()", "after u8g2.setBusClock()");
*/
    }
    void startDisplay(const uint8_t *font) {
      u8g2_prepare();
      u8g2.clearBuffer();
      u8g2.setFont(font);
    }
    void endDisplay() {
      u8g2.sendBuffer();
    }
    void test() {
      for (u8g2_uint_t h = 97; h >= 95; h--) {
        u8g2.clear();
        delay(1000);
        startDisplay(u8g2_font_fur11_tf);
        u8g2.drawFrame(0, 0, getWidth(), h);
        String s(h);
        s.concat(" (full)");
        u8g2.drawUTF8(8, 32, s.c_str());
        endDisplay();
        delay(2000);
      }
    }
    int getHeight() {
      return 96; // ??? why does u8g2.getHeight() return 128 ???
    }
    int getWidth() {
      return u8g2.getWidth();
    }
};
OLEDWrapper* oledWrapper = new OLEDWrapper();

int handleCmd(String command) {
  if (command.equals("ping")) {
    Particle.publish("ping", "pong");
  } else {
    String s("Received unknown command: '");
    s.concat(command);
    s.concat("'");
    Particle.publish("error", s);
    return -1;
  }
  return 1;
}

void setup() {
  delay(3000);
  Particle.function("handleCmd", handleCmd);
  publishEvent("setup", "--------------------------------------");
  oledWrapper->startup();
}

int nLoops = 0;
void loop() {
  if (nLoops == 0) {
    publishEvent("loop", String(nLoops));
    delay(5000);
  }
  nLoops++;
//  oledWrapper->test();
}
