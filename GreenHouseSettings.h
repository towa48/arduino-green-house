#ifndef GREEN_HOUSE_SETTINGS_H
#define GREEN_HOUSE_SETTINGS_H

#include <Wire.h>

struct ValveSettings {
  uint8_t percent;
  uint8_t hour;
  uint8_t minute;
  uint32_t delay; // min
};

#endif
