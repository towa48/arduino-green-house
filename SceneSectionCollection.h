#ifndef SCENE_SECTION_COLLECTION_H
#define SCENE_SECTION_COLLECTION_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "SceneSection.h"

class SceneSectionCollection {
public:
    SceneSectionCollection();
    ~SceneSectionCollection();
    SceneSection* operator[](size_t pos);
    size_t size();
    void push(SceneSection *item);
    size_t find(SceneSection *seek);
    void clear();
private:
    size_t _size;
    size_t _maxSize;
    SceneSection** _arr;
};

#endif
