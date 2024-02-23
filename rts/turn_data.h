#pragma once

#include <instream.h>
#include <outstream.h>
#include <user.h>


#include "command_list.h"

class TurnData {
  public:
    CommandList commandList;

    TurnData() = default;

    TurnData(uint32_t randomValue_, uint32_t crc_, const CommandList &commandList_)
        : randomValue(randomValue_), crc(crc_), commandList(commandList_) {}

    void read(instream &stream) {
      commandList.read(stream);
    }
    void write(outstream &stream) {
      commandList.write(stream);
    }

    uint32_t getRandomValue() const { return randomValue; }
    uint32_t getCrc() const { return crc; }

  private:
    uint32_t randomValue = -1;
    uint32_t crc = -1;
};