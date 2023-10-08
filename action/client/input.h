#pragma once

#include <deque>

#include "move_list.h"

class Input {
  public:
    static Input &get() {
        static Input instance;
        return instance;
    }

    // void process(Action, int key_code)

    MoveList moveList;
};