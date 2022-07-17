#ifndef GREEN_HOUSE_SETTINGS_H
#define GREEN_HOUSE_SETTINGS_H

#include <Wire.h>

struct ValveSettings {
  uint8_t percent;
  uint8_t hour;
  uint8_t minute;
  uint32_t delay; // min
};

struct ValveTestSettings {
  uint8_t percent;
};

enum VALVE_STATE { S_NONE, VALVE_OPEN, VALVE_CLOSE };

#endif
