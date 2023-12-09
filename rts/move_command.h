#pragma once

#include "command.h"

struct Point3 {
  float x;
  float y;
  float z;
};

class MoveCommand : public Command {
  public:
    using ptr = std::shared_ptr<MoveCommand>;

    static ptr create(NetId net_id, const Point3 &target) {
        ptr command;
        Object::ptr object = Network::get().linker.get(net_id);
        UserId userId = Network::get().getUserId();

        // if (object && object->getType() == Object::type && object->getUserId() == userId) {
            command = std::make_shared<MoveCommand>();
            command->netId = net_id;
            command->userId = userId;
            command->target = target;
        // }
        return command;
    }

  private:
    Point3 target;

    void process() {
        // Object::ptr object = Network::get().linker.get(net_id);
        // if (object && object->getType() == Object::type && object->getUserId() == userId) {
        //     penguin->applyCommand(target);
        // }
    }
};