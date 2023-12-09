#pragma once

#include <math.h>
#include <memory>

#include "common/ball_base.h"
#include "user.h"
#include "linker.h"

class InputState;

namespace server {
class Ball final : public BallBase {
  public:
    using ptr = std::shared_ptr<Ball>;

    UserId userId;

    Ball() = default;
    void update() override;

  private:
    NetId netId;

    static Ball::ptr create();
    static Object::ptr base_create() { return create(); }

    friend class Genesis<ObjTypeId, Object::ptr (*)()>;

    static void static_init() {
        static_constructor<&Ball::static_init>::c.run();
        Ball::Factory::add<Object, Ball>();
    }

    void process(const InputState &input, float time_delta);

    void simulate(const float time_delta);
};
}   // namespace server