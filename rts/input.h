#pragma once

#include "command_list.h"

class Input {
  public:
    static Input& get() {
        static Input instance;
        return instance;
    }

    CommandList commandList;

  private:
    Input() {}
};