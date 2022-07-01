#include "DHT.h"
#include "GreenHouseSensors.h"

#define DHTTYPE DHT22

GreenHouseSensors::GreenHouseSensors(uint8_t dhtPin, RTC_DS3231 rtc) :
    _dht(dhtPin, DHTTYPE),
    _rtc(rtc),
    _scanDelay(TimeSpan(60)), // seconds
    _data({ .temperature=0, .humidity=0 })
{
}

void GreenHouseSensors::begin() {
    _dht.begin();
}

SensorsData GreenHouseSensors::read() {
    // read temperature and humidity every X sec
    DateTime now = _rtc.now();
    if (_lastScan + _scanDelay < now) {
        _data.humidity = _dht.readHumidity();
        _data.temperature = _dht.readTemperature();
        _lastScan = now;
    }

    return _data;
}
