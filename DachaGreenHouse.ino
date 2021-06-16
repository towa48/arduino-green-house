/*!
 * 4 pin I2C SSD1306 display
 */

#include "RTClib.h"
#include "DHT.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

enum ButtonType { NONE, LEFT, UP, OK, DOWN, RIGHT };
ButtonType buttonPressed = NONE;

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
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  display.clearDisplay();
  printSensors(t,h);

  DateTime now = rtc.now();
  printTime(now);
  display.display();

  delay(1000);

  //Serial.println("delay");
  //Serial.write();
}

void printSensors(float t, float h) {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(3,5);
  display.print("T:");
  display.print(round(t));
  display.setCursor(42,5);
  display.print("H:");
  display.print(round(h));
}

void printTime(DateTime now) {
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(9,24); // 9,24
  display.print(twoDigitsFromByte(now.hour()));
  display.setCursor(57,24);
  display.print(":");
  display.setCursor(78,24);
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

  if (buttonPressed == LEFT) {
    digitalWrite(VALVE_OPEN_PIN, LOW);
  } else if (buttonPressed == RIGHT) {
    digitalWrite(VALVE_CLOSE_PIN, LOW);
  } else {
    digitalWrite(VALVE_OPEN_PIN, HIGH);
    digitalWrite(VALVE_CLOSE_PIN, HIGH);
  }
}
