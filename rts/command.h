#pragma once

#include <memory>
#include <instream.h>
#include <linker.h>
#include <outstream.h>
#include <user.h>

#include "network.h"


class Command {
  public:
    using ptr = std::shared_ptr<Command>;
    enum class Type { Invalid, Attack, Move };

    Command() : type(Type::Invalid) {}

    virtual void write(outstream &stream) = 0;
    virtual void process() = 0;

  protected:
    Type type;
    NetId netId;
    UserId userId;

    virtual void read(instream &stream) = 0;
};