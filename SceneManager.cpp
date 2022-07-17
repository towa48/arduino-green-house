#include "RTClib.h"
#include "SceneManager.h"
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

        _display.setCursor(9,30);
        DisplayHelper::leadingZeroes(_display, now.hour(), 2); // 16
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

        _display.setCursor(29,30);
        DisplayHelper::leadingZeroes(_display, now.minute(), 2);
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

        _display.setCursor(9,50);
        DisplayHelper::leadingZeroes(_display, now.day(), 2);
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

        _display.setCursor(29,50);
        DisplayHelper::leadingZeroes(_display, now.month(), 2); // 16
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

        _display.setCursor(49,50);
        _display.print(now.year()); // 31
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
    _display.setCursor(25,30);
    _display.print(":"); // 4

    // day.month.year separator
    _display.setCursor(25,50);
    _display.print("."); // 4
    _display.setCursor(45,50);
    _display.print(".");

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
        _display.setCursor(64,30);
        _display.print(_settings->percent); // 16
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
    _display.setTextSize(1);
    _display.setTextColor(WHITE);

    _display.setCursor(9,30);
    _display.print("Открыть:");

    _display.setCursor(88,30);
    _display.print("%");

    _display.setCursor(9,50);
    _display.print("OK - Применить");

    Scene::render(blink);
}

// --------------------
// SceneValveSettings
//--------------------

class ValveSettingsPercentSection : public SceneSection {
public:
    ValveSettingsPercentSection(Adafruit_SSD1306 display, ValveSettings* settings) :
        SceneSection(display),
        _settings(settings)
    {
    }

    void render() override {
        _display.setCursor(42,30);
        _display.print(_settings->percent); // 16
    }
private:
    ValveSettings* _settings;
};

class ValveSettingsDelaySection : public SceneSection {
public:
    ValveSettingsDelaySection(Adafruit_SSD1306 display, ValveSettings* settings) :
        SceneSection(display),
        _settings(settings)
    {
    }

    void render() override {
        _display.setCursor(75,30);
        DisplayHelper::leadingZeroes(_display, _settings->delay, 2); // 16
    }
private:
    ValveSettings* _settings;
};

class ValveSettingsHoursSection : public SceneSection {
public:
    ValveSettingsHoursSection(Adafruit_SSD1306 display, ValveSettings* settings) :
        SceneSection(display),
        _settings(settings)
    {
    }

    void render() override {
        _display.setCursor(57,50);
        DisplayHelper::leadingZeroes(_display, _settings->hour, 2); // 16
    }
private:
    ValveSettings* _settings;
};

class ValveSettingsMinutesSection : public SceneSection {
public:
    ValveSettingsMinutesSection(Adafruit_SSD1306 display, ValveSettings* settings) :
        SceneSection(display),
        _settings(settings)
    {
    }

    void render() override {
        _display.setCursor(77,50);
        DisplayHelper::leadingZeroes(_display, _settings->minute, 2);
    }
private:
    ValveSettings* _settings;
};

SceneValveSettings::SceneValveSettings(Adafruit_SSD1306 display, ValveSettings* settings, const char* title) :
    Scene(display, title, true)
{
    _sections.push(new ValveSettingsPercentSection(display, settings));
    _sections.push(new ValveSettingsDelaySection(display, settings));
    _sections.push(new ValveSettingsHoursSection(display, settings));
    _sections.push(new ValveSettingsMinutesSection(display, settings));
};

void SceneValveSettings::render(bool blink) {
    _display.setTextSize(1);
    _display.setTextColor(WHITE);

    // first line
    _display.setCursor(9,30);
    _display.print("Откр:");

    _display.setCursor(58,30);
    _display.print("%");

    _display.setCursor(100,30);
    _display.print("мин");

    // second line
    _display.setCursor(9,50);
    _display.print("Время:");

    // Hours:minutes
    _display.setCursor(73,50);
    _display.print(":"); // 4

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
    _scenes = new Scene*[6] {
        new SceneHome(display, rtc, state),
        new SceneDateTimeSettings(display, rtc),
        new SceneValveTest(display, &state.valveATest, "КРАН А ТЕСТ"),
        new SceneValveTest(display, &state.valveBTest, "КРАН Б ТЕСТ"),
        new SceneValveSettings(display, &state.valveASettings, "КРАН А"),
        new SceneValveSettings(display, &state.valveBSettings, "КРАН Б")
    };

    _currentScene = 0;
    _dirty = false;
}

void SceneManager::updateDisplay() {
    auto now = millis();
    Scene* scene = _scenes[_currentScene];

    // reset
    if (_lastBlinkTime - now > 10000) {
        _lastBlinkTime = 0;
    }

    if (scene->editable()) {
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
    scene->render(_blink);
    _display.display();
}

void SceneManager::reset() {
    if (!_dirty) {
        return;
    }

    _currentScene = 0;
    _dirty = false;
}

bool SceneManager::idle() {
    return !_dirty;
}

void SceneManager::next() {
    Scene* scene = _scenes[_currentScene];
    if (!scene->editable() || !scene->nextState()) {
        // TODO: change scene
        _dirty = true;
    }

  // TODO: implement correct scene switch

  //if (buttonPressed == LEFT && menuState.current == firstMenu) {
  //  menuState.current = lastMenu;
  //} else if (buttonPressed == RIGHT && menuState.current == lastMenu) {
  //  menuState.current = firstMenu;
  //} else if (buttonPressed == LEFT) {
  //  menuState.current = static_cast<MenuType>(static_cast<int>(menuState.current) - 1);
  //} else if (buttonPressed == RIGHT) {
  //  menuState.current = static_cast<MenuType>(static_cast<int>(menuState.current) + 1);
  //}
}

void SceneManager::prev() {
    Scene* scene = _scenes[_currentScene];
    if (!scene->editable() || !scene->prevState()) {
        // TODO: change scene
        _dirty = true;
    }
}

void SceneManager::inc() {
    Scene* scene = _scenes[_currentScene];
    if (!scene->editable()) {
        return;
    }

    // TODO: call inc
}

void SceneManager::dec() {
    Scene* scene = _scenes[_currentScene];
    if (!scene->editable()) {
        return;
    }

    // TODO: call dec
}

void SceneManager::ok() {
    Scene* scene = _scenes[_currentScene];
    if (!scene->editable()) {
        return;
    }

    // TODO: call ok
}


// TODO: implement inc(), dec(), ok() for manager
/*
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
}*/
