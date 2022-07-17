#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <Wire.h>

enum CommandType { C_NONE, VALVEA_OPEN_25, VALVEA_OPEN_50, VALVEA_OPEN_75, VALVEA_OPEN_100, VALVEA_CLOSE, VALVEB_OPEN_25, VALVEB_OPEN_50, VALVEB_OPEN_75, VALVEB_OPEN_100, VALVEB_CLOSE };

class CommandManager {
public:
    CommandManager();
    void checkQueued();
    void exec(CommandType type);
private:

};

#endif