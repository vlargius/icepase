#pragma once

#include <deque>

#include "move_list.h"
#include "timer.h"


class Input {
  public:
    static Input &get() {
        static Input instance;
        return instance;
    }

    void update() {
      moveList.add(InputState{}, timing::current());
    }

    // void process(Action, int key_code)

    MoveList moveList;
};