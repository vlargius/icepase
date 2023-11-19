#pragma once

#include "instream.h"
#include "outstream.h"

class InputState {
  public:
    float getLateral() const { return right - left; }
    float getLongitudinal() const { return forward - backward; }
    bool getIsShooting() const { return isShooting; }

    template <class Stream> void serialize(Stream &stream) {
        stream.serialize(left);
        stream.serialize(right);
        stream.serialize(forward);
        stream.serialize(backward);
        stream.serialize(isShooting);
    }

    friend class Input;

  private:
    float left = 0.f;
    float right = 0.f;
    float forward = 0.f;
    float backward = 0.f;
    bool isShooting = false;
};