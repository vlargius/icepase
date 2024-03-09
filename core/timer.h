#pragma once

#include <chrono>

#include "static_constructor.h"

class Timer {
  public:
    Timer(float duration_ = 0.f) : duration(duration_) {}
    bool elapsed() const;
    float start() const { return startTime; }

    float duration = 0.f;
    void reset();

  private:
    float startTime = -1.f;
    float endTime = -1.f;
};

namespace timing {
using clock = std::chrono::high_resolution_clock;
namespace {
const clock::time_point start = clock::now();
float deltaTime = 0.f;
// frame lock at 60fps
const float frameDuration = 0.0166666667f;
}
static inline Timer frameTimer{frameDuration};

inline double current() {
    const auto now = clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    return static_cast<double>(ms) / 1000;
}

inline uint64_t current_ms() {
    const auto now = clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
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
