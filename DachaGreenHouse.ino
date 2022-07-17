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

// Scene reset params
unsigned int sceneDelay = (unsigned int)120000; // ms
unsigned long changeTime = 0;

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
  if (!sceneManager.idle() && changeTime + sceneDelay < millis()) {
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
