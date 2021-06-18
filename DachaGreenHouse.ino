/*!
 * 4 pin I2C SSD1306 display
 */

#include "RTClib.h"
#include "DHT.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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
#define DHTTYPE DHT22

#define VALVE_OPEN_PIN 7
#define VALVE_CLOSE_PIN 8
#define BUTTON_PIN 0

//uint32_t emptyTimestamp = 946684800; // 1 jan 2000
//DateTime emptyDate = new DateTime(emptyTimestamp);

enum ButtonType { NONE, LEFT, UP, OK, DOWN, RIGHT };
ButtonType buttonPressed = NONE;

enum MenuType { INFO, HOURS, MINUTES, DAY, MONTH, YEAR, VALVEA_TEST, VALVEB_TEST, VALVEA_PERCENTAGE, VALVEA_HOURS, VALVEA_MINUTES, VALVEB_PERCENTAGE, VALVEB_HOURS, VALVEB_MINUTES };
MenuType firstMenu = INFO;
MenuType lastMenu = VALVEB_MINUTES;
MenuType currentMenu = INFO;
unsigned long menuChangeTime;
unsigned int menuDelay = 120*1000; // ms
bool blink = false;
unsigned long blinkTime;
unsigned int blinkDelay = 500; // ms

TimeSpan scanDelay = new TimeSpan(60); // seconds
DateTime lastScan; // 1 jan 2000
float temperature = 0;
float humidity = 0;

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Started!");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { // Address 0x3D for 128x64
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }

  dht.begin();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

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

  attachInterrupt(0, swap, RISING);
  delay(3000); // init sensors
}

void loop() {
  //Serial.println(lastScan.unixtime());
  //Serial.println(lastScan.isValid());

  // read temperature and humidity every X sec
  DateTime now = rtc.now();
  if (lastScan + scanDelay < now) {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    lastScan = now;
  }

  if (currentMenu == NONE) {
    currentMenu = INFO;
  }

  // reset menu after timeout
  if (currentMenu != INFO && menuChangeTime + menuDelay < millis()) {
    currentMenu = INFO;
  }

  updateDisplay(temperature, humidity);

  if (currentMenu != INFO && buttonPressed != LEFT && buttonPressed != RIGHT) {
    doMenuAction(currentMenu, buttonPressed);
    buttonPressed = NONE;  
  }

  delay(200);

  //Serial.println("delay");
  //Serial.write();
}

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
}

void updateDisplay(float t, float h) {
  DateTime now = rtc.now();

  display.clearDisplay();

  if (currentMenu == INFO) {
    printSensors(t,h);
    printTime(now);
  } else {
    printMenu(currentMenu);
  }
  
  display.display();
}

void printMenu(MenuType m) {
  auto now = millis();
  // reset
  if (blinkTime - now > 10000) {
      blinkTime = 0;
  }

  if (blink && blinkTime + blinkDelay < now) {
    blink = false;
    blinkTime = now;
  } else if (!blink && blinkTime + blinkDelay < now) {
    blink = true;
    blinkTime = now;
  }

  if (m == HOURS || m == MINUTES || m == DAY || m == MONTH || m == YEAR) {
    printMenuTitle("ЧАСЫ");
    printDateMenu(m, blink);
  } else if (m == VALVEA_TEST) {
    printMenuTitle("КРАН А ТЕСТ");
  } else if (m == VALVEB_TEST) {
    printMenuTitle("КРАН Б ТЕСТ");
  } else if (m == VALVEA_PERCENTAGE || m == VALVEA_HOURS || m == VALVEA_MINUTES) {
    printMenuTitle("КРАН А");
  } else if (m == VALVEB_PERCENTAGE || m == VALVEB_HOURS || m == VALVEB_MINUTES) {
    printMenuTitle("КРАН Б");
  }
}

void printDateMenu(MenuType m, bool blink) {
  DateTime now = rtc.now();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (m != HOURS || !blink) {
    display.setCursor(9,30);
    display.print(twoDigitsFromByte(now.hour())); // 16
  }
  display.setCursor(25,30);
  display.print(":"); // 4
  if (m != MINUTES || !blink) {
    display.setCursor(29,30);
    display.print(twoDigitsFromByte(now.minute()));
  }

  if (m != DAY || !blink) {
    display.setCursor(9,50);
    display.print(twoDigitsFromByte(now.day()));
  }
  display.setCursor(25,50);
  display.print("."); // 4
  if (m != MONTH || !blink) {
    display.setCursor(29,50);
    display.print(twoDigitsFromByte(now.month())); // 16
  }
  display.setCursor(45,50);
  display.print(".");
  if (m != YEAR || !blink) {
    display.setCursor(49,50);
    display.print(now.year()); // 31
  }
}

void printMenuTitle(String title) {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(3,10);
  display.print(title);
}

void printSensors(float t, float h) {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(3,10);
  display.print("T:");
  display.print(round(t));
  display.setCursor(42,10);
  display.print("H:");
  display.print(round(h));
}

void printTime(DateTime now) {
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(9,50); // 9,24
  display.print(twoDigitsFromByte(now.hour()));
  display.setCursor(60,50);
  display.print(":");
  display.setCursor(73,50);
  display.print(twoDigitsFromByte(now.minute()));
}

String twoDigitsFromByte(byte value) {
  if (value < 10) {
    return "0"+String(value);
  }
  return String(value);
}

void swap() {
  int buttonState = analogRead(BUTTON_PIN);
  buttonPressed = NONE;

  if (buttonState >= 550){buttonPressed = LEFT;}
  if (buttonState >= 400 && buttonState < 550){buttonPressed = UP;}
  if (buttonState >= 300 && buttonState < 400){buttonPressed = OK;}
  if (buttonState >= 220 && buttonState < 300){buttonPressed = DOWN;}
  if (buttonState >= 150 && buttonState < 220){buttonPressed = RIGHT;}

  //Serial.println(buttonState);
  if (buttonPressed == LEFT || buttonPressed == RIGHT) {
    changeMenu(buttonPressed);
    
    // digitalWrite(VALVE_OPEN_PIN, LOW);
  } //else if (buttonPressed == RIGHT) {
    //digitalWrite(VALVE_CLOSE_PIN, LOW);
  //} else {
  //  digitalWrite(VALVE_OPEN_PIN, HIGH);
  //  digitalWrite(VALVE_CLOSE_PIN, HIGH);
  //}
}

void changeMenu(ButtonType buttonPressed) {
  if (buttonPressed == LEFT && currentMenu == firstMenu) {
    currentMenu = lastMenu;
  } else if (buttonPressed == RIGHT && currentMenu == lastMenu) {
    currentMenu = firstMenu;
  } else if (buttonPressed == LEFT) {
    currentMenu = static_cast<MenuType>(static_cast<int>(currentMenu) - 1);
  } else if (buttonPressed == RIGHT) {
    currentMenu = static_cast<MenuType>(static_cast<int>(currentMenu) + 1);
  }

  menuChangeTime = millis();
}
