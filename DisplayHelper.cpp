#include <Wire.h>
#include "DisplayHelper.h"

namespace DisplayHelper {
    void leadingZeroes(Adafruit_SSD1306 display, uint8_t value, size_t points) {
        char fmt[128];
        sprintf(fmt, "%%0%dd", points);

        char *buf = new char[points];
        sprintf(buf, fmt, value);
        display.print(buf);
        delete []buf;
    }
}
