#ifndef SCENE_H
#define SCENE_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "SceneSectionCollection.h"

/**
 * @brief Base class of the screen scene
 */
class Scene {
public:
    Scene(Adafruit_SSD1306 display);

    /**
     * @brief Render screen for particular scene
     */
    virtual void render();

    /**
     * @brief Change to the next substate. Can be used for scene menu blinking.
     * 
     * @return true when next substate is possible, false overwise
     */
    virtual bool nextState();

    /**
     * @brief Change to the previous substate. Can be used for scene menu blinking.
     * 
     * @return true when next substate is possible, false overwise
     */
    virtual bool prevState();
protected:
    Adafruit_SSD1306 _display;
    SceneSectionCollection _sections;
    size_t _currentSectionIndex;
    bool _editable;
};

#endif
