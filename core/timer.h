#pragma once

#include <chrono>

#include "static_constructor.h"

namespace timing {

namespace {
// const float frameDuration = 0.0166666667f;   // frame lock at 60fps
const float frameDuration = 1.f;
const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

float deltaTime = 0.f;
double frameStartTime = 0.0;
}   // namespace

double current() {
    const auto now = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    return static_cast<double>(ms) / 1000;
}

float getDeltaTime() { return deltaTime; }

float getFrameStartTime() { return frameStartTime; }

void update() {
    double currentTime = current();
    deltaTime = currentTime - frameStartTime;

    while (deltaTime < frameDuration) {
        currentTime = current();
        deltaTime = currentTime - frameStartTime;
    }

    frameStartTime = currentTime;
}

};   // namespace timing

class Timer {
  public:
    Timer(float duration_ = 0.f) : duration(duration_) {}
    bool elapsed() const { return timing::current() > endTime; }

    void setDuration(float value) { duration = 0.f; }
    void reset() { endTime = timing::current() + duration; }

  private:
    float endTime = -1.f;
    float duration = 0.f;
};
