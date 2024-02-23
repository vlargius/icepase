#pragma once

#include "command_list.h"

class Network;

class Input {
  public:

    enum class Action { Press, Repeat, Release };

    static Input& get() {
        static Input instance;
        return instance;
    }

    CommandList commandList;

    void process(Action action, int key_code);
    void click(int32_t x, int32_t y, uint8_t button);

    void update();
    void clear();

  private:
    Input();

    void addCommand(float x, float y);
};