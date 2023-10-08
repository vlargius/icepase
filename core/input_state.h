#pragma once

#include "instream.h"
#include "outstream.h"

class InputState {
  public:
    float getHorizontal() const { return right - left; }
    float getVeretical() const { return up - down; }
    bool getIsShooting() const { return isShooting; }

    template <class Stream> void serialize(Stream &stream) {
        stream.serialize(left);
        stream.serialize(right);
        stream.serialize(up);
        stream.serialize(down);
        stream.serialize(isShooting);
    }

  private:
    float left = 0.f;
    float right = 0.f;
    float up = 0.f;
    float down = 0.f;
    bool isShooting = false;
};