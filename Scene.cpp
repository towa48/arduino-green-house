#include "Scene.h"

Scene::Scene(Adafruit_SSD1306 display, const char* title) :
    _display(display),
    _editable(false),
    _title(title)
{
}

Scene::Scene(Adafruit_SSD1306 display, const char* title, bool editable) :
    _display(display),
    _editable(editable),
    _title(title)
{
}

void Scene::render(bool blink) {
    if (strlen(_title) > 0) {
        printTitle(_title);
    }

    const size_t length = _sections.size();
    if (length == 0) {
        // do nothing
        return;
    }

    for(int i=0; i<length; i++) {
        if (_editable && _currentSectionIndex == i && !blink) {
            // dont render editable section
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

bool Scene::editable() {
    return _editable;
}

void Scene::printTitle(const char* title) {
    _display.setTextSize(1);
    _display.setTextColor(WHITE);

    _display.setCursor(3,10);
    _display.print(title);
}
