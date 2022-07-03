#ifndef SCENE_SECTION_H
#define SCENE_SECTION_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

class SceneSection {
public:
    SceneSection(Adafruit_SSD1306 display);
    virtual void render();
protected:
    Adafruit_SSD1306 _display;
};

#endif