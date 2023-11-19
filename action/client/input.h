#pragma once

#include <deque>
#include <SDL_keycode.h>

#include "move_list.h"
#include "timer.h"

class Input {
  public:
    static Input &get() {
        static Input instance;
        return instance;
    }

    enum class Action { Press, Repeat, Release };

    static void convert(Action action, float& value) {
        if (action == Action::Press)
          value = 1.f;
        else if (action == Action::Release)
          value = 0.f;
    }

    static void convert(Action action, bool& value) {
        if (action == Action::Press)
          value = true;
        else if (action == Action::Release)
          value = false;
    }

    void process(Action action, int key_code) {
        switch (key_code) {
        case SDLK_a:
            convert(action, current.left);
            break;
        case SDLK_d:
            convert(action, current.right);
            break;
        case SDLK_w:
            convert(action, current.forward);
            break;
        case SDLK_s:
            convert(action, current.backward);
            break;
        case SDLK_KP_BACKSPACE:
            convert(action, current.isShooting);
            break;
        }
    }

    void update() {
      moveList.add(current, timing::current());
    }

    MoveList moveList;
private:
  InputState current;
};