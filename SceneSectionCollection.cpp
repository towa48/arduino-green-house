#include "SceneSectionCollection.h"

SceneSectionCollection::SceneSectionCollection() : _size(0), _maxSize(5) {
    _arr = new SceneSection*[_maxSize];
}

SceneSectionCollection::~SceneSectionCollection() {
    for(int i=0; i<_size; i++) {
        delete _arr[i];
    }
    delete[] _arr;
}

SceneSection* SceneSectionCollection::operator[](size_t pos) {
    if (pos > _size-1 || pos < 0) {
    return NULL;
    }
    return _arr[pos];
}

size_t SceneSectionCollection::size() {
    return _size;
}

void SceneSectionCollection::push(SceneSection *item) {
    if (_size == _maxSize) {
        // allocate mem for new array
        size_t newSize = 2*_maxSize;
        SceneSection** newArr = new SceneSection*[newSize];

        for(int i=0; i<_size; i++) {
            newArr[i]=_arr[i];
        }

        delete[] _arr;
        _arr = newArr;
        _maxSize = newSize;
    }

    _arr[_size++] = item;
}

size_t SceneSectionCollection::find(SceneSection *seek) {
    if (_size == 0) {
        return -1;
    }

    for(int i=0; i<_size; i++) {
        if (_arr[i] == seek) {
            return i;
        }
    }

    return -1;
}

void SceneSectionCollection::clear() {
    _size=0;
    _maxSize=5;
    delete[] _arr;

    _arr = new SceneSection*[_maxSize];
}
