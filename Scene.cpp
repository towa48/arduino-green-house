#include "Scene.h"

Scene::Scene(Adafruit_SSD1306 display) :
    _display(display),
    _editable(false)
{
}

void Scene::render() {
    const size_t length = _sections.size();
    if (length == 0) {
        // do nothing
        return;
    }

    for(int i=0; i<length; i++) {
        if (_editable && _currentSectionIndex == i) {
            // TODO: blink if needed
            _sections[i]->render();
        } else {
            _sections[i]->render();
        }
    }
}

bool Scene::nextState() {
    if (!_editable || _currentSectionIndex == _sections.size() - 1) {
        return false;
    }

    _currentSectionIndex++;
    return true;
}

bool Scene::prevState() {
    if (!_editable || _currentSectionIndex == 0) {
        return false;
    }

    _currentSectionIndex--;
    return true;
}
