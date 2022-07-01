#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

namespace DisplayHelper {
    void leadingZeroes(Adafruit_SSD1306 display, uint8_t value, size_t points);
}

#endif