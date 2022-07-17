#include "CommandManager.h"

CommandManager::CommandManager() {
    CommandType queuedCommand = C_NONE;
}

void CommandManager::checkQueued() {
    DateTime now = rtc.now();

  if (state.valveA == S_NONE && now.hour() == valveASettings.hour && now.minute() == valveASettings.minute) {
    switch(valveASettings.percent) {
      case 25:
        queuedCommand = VALVEA_OPEN_25;
        state.valveA = VALVE_OPEN;
        state.valveALastOpen = now;
        break;
      case 50:
        queuedCommand = VALVEA_OPEN_50;
        state.valveA = VALVE_OPEN;
        state.valveALastOpen = now;
        break;
      case 75:
        queuedCommand = VALVEA_OPEN_75;
        state.valveA = VALVE_OPEN;
        state.valveALastOpen = now;
        break;
      default:
        queuedCommand = VALVEA_OPEN_100;
        state.valveA = VALVE_OPEN;
        state.valveALastOpen = now;
        break;
    }
  } else if (state.valveA == VALVE_OPEN && state.valveALastOpen + TimeSpan(valveASettings.delay * 60) < now) {
    queuedCommand = VALVEA_CLOSE;
    state.valveA = S_NONE;
  }

  // DO
  if (queuedCommand != C_NONE) {
    exec(queuedCommand);
    queuedCommand = C_NONE;
  }
}

void exec(CommandType type) {
    byte k_delay;
  unsigned long delay025 = 900;
  bool isValveOpen = false;
  bool isValveClose = false;
  switch(command) {
    case VALVEA_OPEN_25:
      k_delay = 1;
      isValveOpen = true;
      break;
    case VALVEA_OPEN_50:
      k_delay = 2;
      isValveOpen = true;
      break;
    case VALVEA_OPEN_75:
      k_delay = 3;
      isValveOpen = true;
      break;
    case VALVEA_OPEN_100:
      k_delay = 4;
      isValveOpen = true;
      break;
    case VALVEA_CLOSE:
      k_delay = 4;
      isValveClose = true;
  }

  if (isValveOpen) {
    digitalWrite(VALVE_OPEN_PIN, LOW);
    delay(k_delay * delay025);
    digitalWrite(VALVE_OPEN_PIN, HIGH);
    return;
  } else if (isValveClose) {
    digitalWrite(VALVE_CLOSE_PIN, LOW);
    delay(k_delay * delay025);
    digitalWrite(VALVE_CLOSE_PIN, HIGH);
  }
}

