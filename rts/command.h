#pragma once

#include <memory>
#include <user.h>

#include <linker.h>

class instream;
class outstream;

class Command {
  public:
    using ptr = std::shared_ptr<Command>;
    enum class Type { Invalid, Attack, Move, Count };

    static Command::ptr create(instream& stream);

    Command() = default;
    virtual void write(outstream &stream);
    virtual void read(instream &stream) = 0;
    virtual void process() = 0;

    Type type;
    NetId netId;
    UserId userId;
};