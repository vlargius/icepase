#pragma once

#include "command.h"

#include "network.h"

class AttackCommand : public Command {
  public:
    using ptr = std::shared_ptr<AttackCommand>;

    void read(instream &stream) override {}

    void process() override {}
};