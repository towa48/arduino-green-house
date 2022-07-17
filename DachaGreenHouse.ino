/*!
 * 4 pin I2C SSD1306 display
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "RTClib.h"

#include "GreenHouseSettings.h"
#include "SceneManager.h"
#include "GreenHouseSensors.h"
#include "DisplayHelper.h"
#include "CommandManager.h"

// Copy FreeSans6pt8b_cyr.h to libraries\Adafruit-GFX\Fonts
#include <Fonts/FreeSans6pt8b_cyr.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_ADDR   0x3C // OLED display TWI address

// You cant change SDA & SCL pins because they are hardware lock
// A4 - SDA, A5 - SCL
// https://www.arduino.cc/en/reference/wire

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define DHTPIN 4

#define VALVE_OPEN_PIN 7
#define VALVE_CLOSE_PIN 8
#define BUTTON_PIN 0

//uint32_t emptyTimestamp = 946684800; // 1 jan 2000
//DateTime emptyDate = DateTime(emptyTimestamp);

enum ButtonType { NONE, LEFT, UP, OK, DOWN, RIGHT };
ButtonType buttonPressed = NONE;

// TODO: move to sceneManager
enum MenuType { INFO, HOURS, MINUTES, DAY, MONTH, YEAR, VALVEA_TEST, VALVEB_TEST, VALVEA_PERCENTAGE, VALVEA_DELAY, VALVEA_HOURS, VALVEA_MINUTES, VALVEB_PERCENTAGE, VALVEB_DELAY, VALVEB_HOURS, VALVEB_MINUTES };

// Scene reset params
unsigned int sceneDelay = (unsigned int)120000; // ms
unsigned long changeTime = 0;

struct MenuState {
  MenuType current;
};
MenuState menuState = { INFO };

ValveSettings valveASettings { .percent=100, .hour=19, .minute=0, .delay=3 };
ValveSettings valveBSettings { .percent=100, .hour=19, .minute=0, .delay=3 };

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;
GreenHouseSensors sensors(DHTPIN, rtc);

GreenHouseState state;
CommandManager commandManager;
SceneManager sceneManager(display, rtc, state);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Started!");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { // Address 0x3D for 128x64
    Serial.println("SSD1306 allocation failed");
    Serial.flush();
    abort();
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  sensors.begin();

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  display.setFont(&FreeSans6pt8b); // set cyrillic font

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();

  pinMode(VALVE_OPEN_PIN, OUTPUT);
  pinMode(VALVE_CLOSE_PIN, OUTPUT);
  digitalWrite(VALVE_OPEN_PIN, HIGH);
  digitalWrite(VALVE_CLOSE_PIN, HIGH);

  pinMode(BUTTON_PIN, INPUT);

  // TODO: Load struct from EEPROM

  // Clear state
  commandManager.exec(VALVEA_CLOSE);
  commandManager.exec(VALVEB_CLOSE);

  attachInterrupt(0, swapButton, RISING);
  delay(2000); // init sensors
}

void loop() {
  //Serial.println(lastScan.unixtime());
  //Serial.println(lastScan.isValid());

  // reset scene after timeout
  if (menuState.current != INFO && changeTime + sceneDelay < millis()) {
    sceneManager.reset();
  }

  state.sensors = sensors.read();
  sceneManager.updateDisplay();

  switch (buttonPressed)
  {
    case LEFT:
      sceneManager.prev();
      break;
    case RIGHT:
      sceneManager.next();
      break;
    case UP:
      sceneManager.inc();
      break;
    case DOWN:
      sceneManager.dec();
      break;
    case OK:
      sceneManager.ok();
      break;
  }

  // Reset button state beacause all managers already notified
  if (buttonPressed != NONE) {
    buttonPressed = NONE;
  }

  commandManager.checkQueued();
  delay(200);

  //Serial.println("delay");
  //Serial.write();
}

// TODO: implement inc(), dec(), ok() for manager
void doMenuAction(MenuType menu, ButtonType button) {
  DateTime now = rtc.now();
  if (menu == HOURS) {
    if (button == UP) {
      uint8_t h = now.hour() + 1;
      h = h > 23 ? 0 : h;
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, now.minute(), 0));
    } else if (button == DOWN) {
      uint8_t h = now.hour() - 1;
      h = h < 0 ? 23 : h;
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, now.minute(), 0));
    }
    return;
  }

  if (menu == MINUTES) {
    if (button == UP) {
      uint8_t m = now.minute() + 1;
      m = m > 59 ? 0 : m;
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), m, 0));
    } else if (button == DOWN) {
      uint8_t m = now.minute() - 1;
      m = m < 0 ? 59 : m;
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), m, 0));
    }
    return;
  }

  if (menu == DAY) {
    if (button == UP) {
      uint8_t d = now.day() + 1;
      d = d > 31 ? 1 : d;
      rtc.adjust(DateTime(now.year(), now.month(), d, now.hour(), now.minute(), 0));
    } else if (button == DOWN) {
      uint8_t d = now.day() - 1;
      d = d < 1 ? 31 : d;
      rtc.adjust(DateTime(now.year(), now.month(), d, now.hour(), now.minute(), 0));
    }
    return;
  }

  if (menu == MONTH) {
    if (button == UP) {
      uint8_t m = now.month() + 1;
      m = m > 12 ? 1 : m;
      rtc.adjust(DateTime(now.year(), m, now.day(), now.hour(), now.minute(), 0));
    } else if (button == DOWN) {
      uint8_t m = now.month() - 1;
      m = m < 1 ? 12 : m;
      rtc.adjust(DateTime(now.year(), m, now.day(), now.hour(), now.minute(), 0));
    }
    return;
  }

  if (menu == YEAR) {
    if (button == UP) {
      uint16_t y = now.year() + 1;
      y = y > 2100 ? 2001 : y;
      rtc.adjust(DateTime(y, now.month(), now.day(), now.hour(), now.minute(), 0));
    } else if (button == DOWN) {
      uint16_t y = now.year() - 1;
      y = y < 2001 ? 2100 : y;
      rtc.adjust(DateTime(y, now.month(), now.day(), now.hour(), now.minute(), 0));
    }
    return;
  }

  if (menu == VALVEA_TEST) {
    if (button == UP && state.valveATest.percent < 100) {
      state.valveATest.percent += 25;
    } else if (button == DOWN && state.valveATest.percent > 0) {
      state.valveATest.percent -= 25;
    }
  } else if (menu == VALVEB_TEST) {
    if (button == UP && state.valveBTest.percent < 100) {
      state.valveBTest.percent += 25;
    } else if (button == DOWN && state.valveBTest.percent > 0) {
      state.valveBTest.percent -= 25;
    }
  }

  if (button == OK && menu == VALVEA_TEST) {
    switch(state.valveATest.percent) {
      case 0:
        commandManager.exec(VALVEA_CLOSE);
        break;
      case 25:
        commandManager.exec(VALVEA_OPEN_25);
        break;
      case 50:
        commandManager.exec(VALVEA_OPEN_50);
        break;
      case 75:
        commandManager.exec(VALVEA_OPEN_75);
        break;
      case 100:
        commandManager.exec(VALVEA_OPEN_100);
        break;
    }
    return;
  } else if (button == OK && menu == VALVEB_TEST) {
    switch(state.valveBTest.percent) {
      case 0:
        commandManager.exec(VALVEB_CLOSE);
        break;
      case 25:
        commandManager.exec(VALVEB_OPEN_25);
        break;
      case 50:
        commandManager.exec(VALVEB_OPEN_50);
        break;
      case 75:
        commandManager.exec(VALVEB_OPEN_75);
        break;
      case 100:
        commandManager.exec(VALVEB_OPEN_100);
        break;
    }
    return;
  }

  // VALVE A SETTINGS

  if (menu == VALVEA_PERCENTAGE) {
    if (button == UP && valveASettings.percent < 100) {
      valveASettings.percent += 25;
    } else if (button == DOWN && valveASettings.percent > 25) {
      valveASettings.percent -= 25;
    }
    return;
  } else if (menu == VALVEA_DELAY) {
    if (button == UP && valveASettings.delay < 360) {
      valveASettings.delay += 1;
    } else if (button == DOWN && valveASettings.delay > 1) {
      valveASettings.delay -= 1;
    }
    return;
  } else if (menu == VALVEA_HOURS) {
    if (button == UP && valveASettings.hour < 23) {
      valveASettings.hour += 1;
    }else if (button == UP && valveASettings.hour >= 23) {
      valveASettings.hour = 0;
    } else if (button == DOWN && valveASettings.hour > 0) {
      valveASettings.hour -= 1;
    }else if (button == DOWN && valveASettings.hour <= 0) {
      valveASettings.hour = 23;
    }
    return;
  } else if (menu == VALVEA_MINUTES) {
    if (button == UP && valveASettings.minute < 59) {
      valveASettings.minute += 1;
    }else if (button == UP && valveASettings.minute >= 59) {
      valveASettings.minute = 0;
    } else if (button == DOWN && valveASettings.minute > 0) {
      valveASettings.minute -= 1;
    }else if (button == DOWN && valveASettings.minute <= 0) {
      valveASettings.minute = 59;
    }
    return;
  }

  // VALVE B SETTINGS

  if (menu == VALVEB_PERCENTAGE) {
    if (button == UP && valveBSettings.percent < 100) {
      valveBSettings.percent += 25;
    } else if (button == DOWN && valveBSettings.percent > 25) {
      valveBSettings.percent -= 25;
    }
    return;
  } else if (menu == VALVEB_DELAY) {
    if (button == UP && valveBSettings.delay < 360) {
      valveBSettings.delay += 1;
    } else if (button == DOWN && valveBSettings.delay > 1) {
      valveBSettings.delay -= 1;
    }
    return;
  } else if (menu == VALVEB_HOURS) {
    if (button == UP && valveBSettings.hour < 23) {
      valveBSettings.hour += 1;
    }else if (button == UP && valveBSettings.hour >= 23) {
      valveBSettings.hour = 0;
    } else if (button == DOWN && valveBSettings.hour > 0) {
      valveBSettings.hour -= 1;
    }else if (button == DOWN && valveBSettings.hour <= 0) {
      valveBSettings.hour = 23;
    }
    return;
  } else if (menu == VALVEB_MINUTES) {
    if (button == UP && valveBSettings.minute < 59) {
      valveBSettings.minute += 1;
    }else if (button == UP && valveBSettings.minute >= 59) {
      valveBSettings.minute = 0;
    } else if (button == DOWN && valveBSettings.minute > 0) {
      valveBSettings.minute -= 1;
    }else if (button == DOWN && valveBSettings.minute <= 0) {
      valveBSettings.minute = 59;
    }
    return;
  }
}

void swapButton() {
  if (changeTime + 200 > millis()) {
    // prevent double click
    return;
  }

  int buttonState = analogRead(BUTTON_PIN);
  buttonPressed = NONE;

  if (buttonState >= 550){buttonPressed = LEFT;}
  if (buttonState >= 400 && buttonState < 550){buttonPressed = UP;}
  if (buttonState >= 300 && buttonState < 400){buttonPressed = OK;}
  if (buttonState >= 220 && buttonState < 300){buttonPressed = DOWN;}
  if (buttonState >= 150 && buttonState < 220){buttonPressed = RIGHT;}

  //Serial.println(buttonState);

  if (buttonPressed != NONE) {
    changeTime = millis();
  }
}
