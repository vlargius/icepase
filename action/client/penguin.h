#pragma once

#include "common/penguin_base.h"

namespace client {

class Penguin final : public PenguinBase {
  public:
    using ptr = std::shared_ptr<Penguin>;

    Penguin() = default;

    void update() override {
      std::cout << "position: " << position.x << ", " << position.y << std::endl;
    }

    void read(instream &stream) override {
        bool hasState;
        stream.read(hasState);
        if (hasState) {
            stream.read(position);
        }
    }

  private:

    static Penguin::ptr create() { return std::make_shared<Penguin>(); }
    static Object::ptr base_create() { return create(); }

    friend class Genesis<ObjTypeId, Object::ptr(*)()>;

    static void static_init() {
      static_constructor<&Penguin::static_init>::c.run();
      Penguin::Factory::add<Object, Penguin>();
    }
};
}   // namespace client