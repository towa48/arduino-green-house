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
    Scene(Adafruit_SSD1306 display, const char* title);

    Scene(Adafruit_SSD1306 display, const char* title, bool editable);

    /**
     * @brief Render screen for particular scene
     * 
     * @param blink Current section should not be displayed if scene is editable
     */
    virtual void render(bool blink);

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

    /**
     * @brief Describe scene as editable
     * 
     * @return true when scene is editable, false overwise
     */
    virtual bool editable();
protected:
    Adafruit_SSD1306 _display;
    SceneSectionCollection _sections;
    size_t _currentSectionIndex;
    bool _editable;
private:
    const char* _title;
    void printTitle(const char* title);
};

#endif
