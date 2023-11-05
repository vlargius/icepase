#pragma once

#include "common/penguin_base.h"
#include "network.h"

namespace server {

class Penguin final : public PenguinBase {
  public:
    using ptr = std::shared_ptr<Penguin>;
    Penguin() = default;

    void update() override {
      position.x += 1.f;
      position.y -= 0.5f;

      Network::get().updateFields(netId, Fields::Position);
    }

  private:
    static Penguin::ptr create() {
        Penguin::ptr penguin = std::make_shared<Penguin>();
        penguin->netId = Network::get().link(penguin);
        return penguin;
    }
    static Object::ptr base_create() { return create(); }

    friend class Genesis<ObjTypeId, Object::ptr (*)()>;

    static void static_init() {
        static_constructor<&Penguin::static_init>::c.run();
        Penguin::Factory::add<Object, Penguin>();
    }

    NetId netId;
};
}   // namespace server