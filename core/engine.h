#pragma once

#include <iostream>

#include "timer.h"
#include "world.h"


class Engine {
public:
    bool isRunning = true;

    void run() {
        setup();
        loop();
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