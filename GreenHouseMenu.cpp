#include "RTClib.h"
#include "GreenHouseMenu.h"
#include "DisplayHelper.h"

// --------------------
// SceneHome
//--------------------

class DateTimeSection : public SceneSection {
public:
    DateTimeSection(Adafruit_SSD1306 display, RTC_DS3231 rtc) : SceneSection(display), _rtc(rtc) {
    }
    void render() override {
        DateTime now = _rtc.now();

        _display.setTextSize(3);
        _display.setTextColor(WHITE);

        _display.setCursor(9,50); // 9,24
        DisplayHelper::leadingZeroes(_display, now.hour(), 2);

        _display.setCursor(58,50);
        _display.print(":");

        _display.setCursor(73,50);
        DisplayHelper::leadingZeroes(_display, now.minute(), 2);
    }
private:
    RTC_DS3231 _rtc;
};

class TemperatureSection : public SceneSection {
public:
    TemperatureSection(Adafruit_SSD1306 display, GreenHouseState state) :
        SceneSection(display),
        _state(state)
    {
    }
    void render() override {
        _display.setCursor(3,10);
        _display.print("T:");
        _display.print(round(_state.sensors.temperature));
    }
private:
    GreenHouseState _state;
};

class HumiditySection : public SceneSection {
public:
    HumiditySection(Adafruit_SSD1306 display, GreenHouseState state) :
        SceneSection(display),
        _state(state)
    {
    }
    void render() override {
        _display.setCursor(42,10);
        _display.print("H:");
        _display.print(round(_state.sensors.humidity));
    }
private:
    GreenHouseState _state;
};

SceneHome::SceneHome(Adafruit_SSD1306 display, RTC_DS3231 rtc, GreenHouseState state) : Scene(display) {
    _sections.push(new DateTimeSection(display, rtc));
    _sections.push(new TemperatureSection(display, state));
    _sections.push(new HumiditySection(display, state));
}

void SceneHome::render() {
    _display.setTextSize(1);
    _display.setTextColor(WHITE);
}

// --------------------
// SceneDateTimeSettings
//--------------------

class SceneDateTimeSettings: public Scene {
};
