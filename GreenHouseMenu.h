#ifndef GREEN_HOUSE_MENU_H
#define GREEN_HOUSE_MENU_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "Scene.h"
#include "GreenHouseSensors.h"

struct ValveTestSettings {
    uint8_t percent = 25;
};

class GreenHouseState {
public:
    SensorsData sensors;
    ValveTestSettings valveATest;
    ValveTestSettings valveBTest;
};

class SceneHome : public Scene {
public:
    SceneHome(Adafruit_SSD1306 display, RTC_DS3231 rtc, GreenHouseState state);
};

class SceneDateTimeSettings: public Scene {
public:
    SceneDateTimeSettings(Adafruit_SSD1306 display, RTC_DS3231 rtc);
    void render(bool blink) override;
};

class SceneValveTest: public Scene {
public:
    SceneValveTest(Adafruit_SSD1306 display, ValveTestSettings* settings, const char* title);
    void render(bool blink) override;
};

const unsigned int BLINK_DELAY = 500; // ms

class SceneManager {
public:
    SceneManager(Adafruit_SSD1306 display, RTC_DS3231 rtc, GreenHouseState state);
    void updateDisplay();
    void next();
    void prev();
    void inc();
    void dec();
    void ok();
private:
    Adafruit_SSD1306 _display;
    RTC_DS3231 _rtc;
    GreenHouseState _state;
    Scene* _currentScene;
    Scene** _scenes;

    bool _blink;
    unsigned long _lastBlinkTime;
};

#endif
