#pragma once

#include <chrono>

#include "static_constructor.h"

class Timer {
  public:
    Timer(float duration_ = 0.f) : duration(duration_) {}
    bool elapsed() const;
    float start() const { return startTime; }

    void setDuration(float value);
    void reset();

  private:
    float duration = 0.f;
    float startTime = -1.f;
    float endTime = -1.f;
};

namespace timing {
using clock = std::chrono::high_resolution_clock;
namespace {
const float frameDuration = 0.0166666667f;   // frame lock at 60fps
// const float frameDuration = .24f;
// const float frameDuration = 1.f;
const clock::time_point start = clock::now();
Timer frameTimer{frameDuration};
float deltaTime = 0.f;
}   // namespace

inline double current() {
    const auto now = clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    return static_cast<double>(ms) / 1000;
}

inline float delta() { return deltaTime; }
inline float frame_start() { return frameTimer.start(); }

inline void update() {
    while (!frameTimer.elapsed()) {
        deltaTime = current() - frameTimer.start();
    }
    frameTimer.reset();
}
};   // namespace timing

inline bool Timer::elapsed() const { return timing::current() > endTime; }
inline void Timer::reset() {
    startTime = timing::current();
    endTime = startTime + duration;
}
