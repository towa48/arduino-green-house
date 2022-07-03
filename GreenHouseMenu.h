#ifndef GREEN_HOUSE_MENU_H
#define GREEN_HOUSE_MENU_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "Scene.h"
#include "GreenHouseSensors.h"

class GreenHouseState {
public:
    SensorsData sensors;
};

class SceneHome : public Scene {
public:
    SceneHome(Adafruit_SSD1306 display, RTC_DS3231 rtc, GreenHouseState state);
    void render() override;
};

#endif
