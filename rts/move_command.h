#pragma once

#include "command.h"

#include "network.h"

class MoveCommand : public Command {
  public:
    using ptr = std::shared_ptr<MoveCommand>;

    static ptr create(NetId net_id, float target_x, float target_y) {
        ptr command;
        Object::ptr object = Network::get().linker.get(net_id);
        UserId userId = Network::get().getUserId();

        if (object && object->getType() == Penguin::type &&
            std::static_pointer_cast<Penguin>(object)->userId == userId) {
            command = std::make_shared<MoveCommand>();
            command->netId = net_id;
            command->userId = userId;
            command->targetX = target_x;
            command->targetY = target_y;
        }
        return command;
    }

    MoveCommand() { type = Type::Move; }

    void write(outstream &stream) override {
        Command::write(stream);
        stream.write(targetX);
        stream.write(targetY);
    }

    float targetX;
    float targetY;

    void read(instream &stream) override {
        stream.read(targetX);
        stream.read(targetY);
    }

    void process() override {
        Object::ptr object = Network::get().linker.get(netId);
        if (object && object->getType() == Penguin::type &&
            std::static_pointer_cast<Penguin>(object)->userId == userId) {
            Penguin::ptr penguin = std::static_pointer_cast<Penguin>(object);
            penguin->target.x = targetX;
            penguin->target.y = targetY;
        }
    }
};