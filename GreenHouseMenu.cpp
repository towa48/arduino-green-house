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

class SensorsSection : public SceneSection {
public:
    SensorsSection(Adafruit_SSD1306 display, GreenHouseState state) :
        SceneSection(display),
        _state(state)
    {
    }

    void render() override {
        _display.setTextSize(1);
        _display.setTextColor(WHITE);

        _display.setCursor(3,10);
        _display.print("T:");
        _display.print(round(_state.sensors.temperature));

        _display.setCursor(42,10);
        _display.print("H:");
        _display.print(round(_state.sensors.humidity));
    }
private:
    GreenHouseState _state;
};

SceneHome::SceneHome(Adafruit_SSD1306 display, RTC_DS3231 rtc, GreenHouseState state) : Scene(display, "") {
    _sections.push(new DateTimeSection(display, rtc));
    _sections.push(new SensorsSection(display, state));
}

// --------------------
// SceneDateTimeSettings
//--------------------

class DateTimeHourSection : public SceneSection {
public:
    DateTimeHourSection(Adafruit_SSD1306 display, RTC_DS3231 rtc) : SceneSection(display), _rtc(rtc) {
    }

    void render() override {
        DateTime now = _rtc.now();

        display.setCursor(9,30);
        DisplayHelper::leadingZeroes(display, now.hour(), 2); // 16
    }
private:
    RTC_DS3231 _rtc;
};

class DateTimeMinuteSection : public SceneSection {
public:
    DateTimeMinuteSection(Adafruit_SSD1306 display, RTC_DS3231 rtc) : SceneSection(display), _rtc(rtc) {
    }
    void render() override {
        DateTime now = _rtc.now();

        display.setCursor(29,30);
        DisplayHelper::leadingZeroes(display, now.minute(), 2);
    }
private:
    RTC_DS3231 _rtc;
};

class DateTimeDaySection : public SceneSection {
public:
    DateTimeDaySection(Adafruit_SSD1306 display, RTC_DS3231 rtc) : SceneSection(display), _rtc(rtc) {
    }
    void render() override {
        DateTime now = _rtc.now();

        display.setCursor(9,50);
        DisplayHelper::leadingZeroes(display, now.day(), 2);
    }
private:
    RTC_DS3231 _rtc;
};

class DateTimeMonthSection : public SceneSection {
public:
    DateTimeMonthSection(Adafruit_SSD1306 display, RTC_DS3231 rtc) : SceneSection(display), _rtc(rtc) {
    }
    void render() override {
        DateTime now = _rtc.now();

        display.setCursor(29,50);
        DisplayHelper::leadingZeroes(display, now.month(), 2); // 16
    }
private:
    RTC_DS3231 _rtc;
};

class DateTimeYearSection : public SceneSection {
public:
    DateTimeYearSection(Adafruit_SSD1306 display, RTC_DS3231 rtc) : SceneSection(display), _rtc(rtc) {
    }
    void render() override {
        DateTime now = _rtc.now();

        display.setCursor(49,50);
        display.print(now.year()); // 31
    }
private:
    RTC_DS3231 _rtc;
};

SceneDateTimeSettings::SceneDateTimeSettings(Adafruit_SSD1306 display, RTC_DS3231 rtc) : Scene(display, "ЧАСЫ", true) {
    _sections.push(new DateTimeHourSection(display, rtc));
    _sections.push(new DateTimeMinuteSection(display, rtc));
    _sections.push(new DateTimeDaySection(display, rtc));
    _sections.push(new DateTimeMonthSection(display, rtc));
    _sections.push(new DateTimeYearSection(display, rtc));
};

void SceneDateTimeSettings::render(bool blink) {
    _display.setTextSize(1);
    _display.setTextColor(WHITE);

    // hours:minutes separator
    display.setCursor(25,30);
    display.print(":"); // 4

    // day.month.year separator
    display.setCursor(25,50);
    display.print("."); // 4
    display.setCursor(45,50);
    display.print(".");

    Scene::render(blink);
}

// --------------------
// SceneValveTest
//--------------------

class ValveTestPercentSection : public SceneSection {
public:
    ValveTestPercentSection(Adafruit_SSD1306 display, ValveTestSettings* settings) :
        SceneSection(display),
        _settings(settings)
    {
    }

    void render() override {
        display.setCursor(64,30);
        display.print(_settings->percent); // 16
    }
private:
    ValveTestSettings* _settings;
};

SceneValveTest::SceneValveTest(Adafruit_SSD1306 display, ValveTestSettings* settings, const char* title) :
    Scene(display, title, true)
{
    _sections.push(new ValveTestPercentSection(display, settings));
};

void SceneValveTest::render(bool blink) {
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(9,30);
    display.print("Открыть:");

    display.setCursor(88,30);
    display.print("%");

    display.setCursor(9,50);
    display.print("OK - Применить");

    Scene::render(blink);
}

// --------------------
// SceneManager
//--------------------

SceneManager::SceneManager(Adafruit_SSD1306 display, RTC_DS3231 rtc, GreenHouseState state) :
    _display(display),
    _rtc(rtc),
    _state(state),
    _blink(false),
    _lastBlinkTime(0)
{
    _scenes = new Scene*[4] {
        new SceneHome(display, rtc, state),
        new SceneDateTimeSettings(display, rtc),
        new SceneValveTest(display, &state.valveATest, "КРАН А ТЕСТ"),
        new SceneValveTest(display, &state.valveBTest, "КРАН Б ТЕСТ")
    }

    _currentScene = _scenes[0];
}

void SceneManager::updateDisplay() {
    auto now = millis();

    // reset
    if (_lastBlinkTime - now > 10000) {
        _lastBlinkTime = 0;
    }

    if (_currentScene->editable()) {
        if (_blink && _lastBlinkTime + BLINK_DELAY < now) {
            _blink = false;
            _lastBlinkTime = now;
        } else if (!_blink && _lastBlinkTime + BLINK_DELAY < now) {
            _blink = true;
            _lastBlinkTime = now;
        }
    } else if (_blink) {
        // reset
        _blink = false;
        _lastBlinkTime = now;
    }

    _display.clearDisplay();
    _currentScene->render(_blink);
    _display.display();
}

void SceneManager::next() {
    if (!_currentScene->editable() || !_currentScene->nextState()) {
        // TODO: change scene
    }
}

void SceneManager::prev() {
    
}

void SceneManager::inc() {
    if (!_currentScene->editable()) {
        return;
    }

    // TODO: call inc
}

void SceneManager::dec() {
    if (!_currentScene->editable()) {
        return;
    }

    // TODO: call dec
}

void SceneManager::ok() {
    if (!_currentScene->editable()) {
        return;
    }

    // TODO: call ok
}

// --------------------
// LEGACY
//--------------------

void printMenu(MenuState menu) {


  MenuType m = menu.current;

  } else if (m == VALVEA_PERCENTAGE || m == VALVEA_DELAY || m == VALVEA_HOURS || m == VALVEA_MINUTES) {
    printMenuTitle("КРАН А");
    printValveSettings(m, menu.blink, valveASettings);
  } else if (m == VALVEB_PERCENTAGE || m == VALVEB_DELAY || m == VALVEB_HOURS || m == VALVEB_MINUTES) {
    printMenuTitle("КРАН Б");
    printValveSettings(m, menu.blink, valveBSettings);
  } else {
    //int unknownMenu = static_cast<int>(menuState.current);
    //printMenuTitle(String(unknownMenu));
  }
}

void printValveSettings(MenuType m, bool blink, ValveSettings settings) {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(9,30);
  display.print("Откр:");
  if ((m != VALVEA_PERCENTAGE && m != VALVEB_PERCENTAGE) || !blink) {
    display.setCursor(42,30);
    display.print(settings.percent); // 16
  }
  display.setCursor(58,30);
  display.print("%");

  if ((m != VALVEA_DELAY && m != VALVEB_DELAY) || !blink) {
    display.setCursor(75,30);
    DisplayHelper::leadingZeroes(display, settings.delay, 2); // 16
  }
  display.setCursor(100,30);
  display.print("мин");

  display.setCursor(9,50);
  display.print("Время:");
  if ((m != VALVEA_HOURS && m != VALVEB_HOURS) || !blink) {
    display.setCursor(57,50);
    DisplayHelper::leadingZeroes(display, settings.hour, 2); // 16
  }
  display.setCursor(73,50);
  display.print(":"); // 4
  if ((m != VALVEA_MINUTES && m != VALVEB_MINUTES) || !blink) {
    display.setCursor(77,50);
    DisplayHelper::leadingZeroes(display, settings.minute, 2);
  }
}

