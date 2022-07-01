#ifndef GREEN_HOUSE_SENSORS_H
#define GREEN_HOUSE_SENSORS_H

#include <Wire.h>

#include "RTClib.h"
#include "DHT.h"

struct SensorsData {
    float temperature;
    float humidity;
};

class GreenHouseSensors {
public:
    GreenHouseSensors (uint8_t dhtPin, RTC_DS3231 rtc);
    void begin();
    SensorsData read();
private:
    DHT _dht;
    RTC_DS3231 _rtc;
    SensorsData _data;
    DateTime _lastScan; // 1 jan 2000
    TimeSpan _scanDelay;
};

#endif
