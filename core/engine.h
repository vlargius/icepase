#pragma once

#include <iostream>

#include "timer.h"
#include "world.h"


class Engine {
public:
    bool isRunning = true;

    int run() {
        setup();
        loop();
        return 0;
    }

protected:
    virtual void setup() {
    };

    virtual void loop() {
        while (isRunning) {
            frame();
            timing::update();
        }
    }

    virtual void frame() {
        World::get().update();
    }
};