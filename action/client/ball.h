#pragma once

#include <common/ball_base.h>
#include <graphics/renderer.h>

namespace client {

class Ball final : public BallBase {
  public:
    using ptr = std::shared_ptr<Ball>;

    Ball() {
      sprite = std::make_shared<graphics::Renderer::Sprite>();
      sprite->texture = graphics::texture_cash::get("ball");
      sprite->scale = 0.3f;
    }

    void update() override {
      sprite->location.x = position.x;
      sprite->location.y = position.y;
    }

    void read(instream &stream) override {
        bool hasState;
        stream.read(hasState);
        if (hasState) {
            stream.read(position);
        }
    }

  private:

    static Ball::ptr create() { return std::make_shared<Ball>(); }
    static Object::ptr base_create() { return create(); }

    friend class Genesis<ObjTypeId, Object::ptr(*)()>;

    static void static_init() {
      static_constructor<&Ball::static_init>::c.run();
      Ball::Factory::add<Ball, Ball>();
    }

    graphics::Sprite::ptr sprite;
};
}   // namespace client