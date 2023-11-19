#pragma once

#include <math.h>
#include <memory>

#include "common/penguin_base.h"
#include "user.h"
#include "linker.h"

class InputState;

namespace server {
class Penguin final : public PenguinBase {
  public:
    using ptr = std::shared_ptr<Penguin>;

    UserId userId;

    Penguin() = default;
    void update() override;

  private:
    NetId netId;

    static Penguin::ptr create();
    static Object::ptr base_create() { return create(); }

    friend class Genesis<ObjTypeId, Object::ptr (*)()>;

    static void static_init() {
        static_constructor<&Penguin::static_init>::c.run();
        Penguin::Factory::add<Object, Penguin>();
    }

    void process(const InputState &input, float time_delta);

    void simulate(const float time_delta);
};
}   // namespace server