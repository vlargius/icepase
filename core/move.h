#pragma once

#include "input_state.h"

class Move {
  public:
    Move() = default;
    Move(const InputState &state_, float timestamp_, float time_delta)
        : state(state_), timestamp(timestamp_), timeDelta(time_delta) {}

    const InputState& getState() const { return state; }
    float getTimestamp() const { return timestamp; }
    float getTimeDelta() const { return timeDelta; }

    template<class Stream>
    void serialize(Stream& stream) {
        state.serialize(stream);
        stream.serialize(timestamp);
        stream.serialize(timeDelta);
    }
  private:
    InputState state;
    float timestamp = -1;
    float timeDelta = -1;
};