#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "Scene.h"
#include "GreenHouseSensors.h"
#include "GreenHouseSettings.h"

class GreenHouseState {
public:
    SensorsData sensors { .temperature=0, .humidity=0 };
    ValveTestSettings valveATest { .percent=25 };
    ValveTestSettings valveBTest { .percent=25 };
    ValveSettings valveASettings { .percent=100, .hour=19, .minute=0, .delay=5 };
    ValveSettings valveBSettings { .percent=100, .hour=19, .minute=0, .delay=5 };

    VALVE_STATE valveA = S_NONE;
    VALVE_STATE valveB = S_NONE;
    DateTime valveALastOpen;
    DateTime valveBLastOpen;
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

class SceneValveSettings: public Scene {
public:
    SceneValveSettings(Adafruit_SSD1306 display, ValveSettings* settings, const char* title);
    void render(bool blink) override;
};

const unsigned int BLINK_DELAY = 500; // ms

class SceneManager {
public:
    SceneManager(Adafruit_SSD1306 display, RTC_DS3231 rtc, GreenHouseState state);
    void updateDisplay();
    void reset();

    void next();
    void prev();
 
    void inc();
    void dec();
    void ok();
private:
    Adafruit_SSD1306 _display;
    RTC_DS3231 _rtc;
    GreenHouseState _state;
    Scene** _scenes;
    unsigned int _currentScene;

    bool _blink;
    unsigned long _lastBlinkTime;
    bool _dirty;
};

#endif
