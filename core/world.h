#pragma once

#include <vector>
#include <ranges>

#include "object.h"


class World {
public:

    static World& get() {
        static World instance;
        return instance;
    }

    template<class T>
    typename T::ptr add() {
        typename T::ptr object = Object::Factory::create<T>();
        objects.push_back(object);
        return object;
    }

    typename Object::ptr add(ObjTypeId type) {
        Object::ptr object = Object::Factory::create(type);
        objects.push_back(object);
        return object;
    }

    void update() {
        for (auto it = objects.begin(); it != objects.end(); ++it) {
            auto& object = *it;
            if (object->isMarkedDestroyed) {
                it = objects.erase(it);
            } else {
                object->update();
            }
        }
    }

private:
    std::vector<Object::ptr> objects;

    World() = default;
};