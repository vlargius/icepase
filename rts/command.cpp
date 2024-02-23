#include "command.h"

#include <outstream.h>

#include "attack_command.h"
#include "move_command.h"

Command::ptr Command::create(instream &stream) {
    Command::ptr command;
    Type type;
    stream.read<int(Type::Count)>(type);
    switch (type) {
    case Type::Attack: {
        command = std::make_shared<AttackCommand>();
        break;
    }
    case Type::Move: {
        command = std::make_shared<MoveCommand>();
        command->type = type;
        stream.read(command->netId);
        stream.read(command->userId);
        command->read(stream);
        break;
    }
    default:
        break;
    }

    return command;
}

void Command::write(outstream &stream) {
    stream.write<int(Type::Count)>(type);
    stream.write(netId);
    stream.write(userId);
}
