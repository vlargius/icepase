#pragma once

#include <common/penguin_base.h>
#include <graphics/renderer.h>

namespace client {

class Penguin final : public PenguinBase {
  public:
    using ptr = std::shared_ptr<Penguin>;

    Penguin() {
      sprite = std::make_shared<graphics::Sprite>();
      sprite->texture = graphics::texture_cash::get("penguin");
      sprite->scale = 0.2f;
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

    static Penguin::ptr create() { return std::make_shared<Penguin>(); }
    static Object::ptr base_create() { return create(); }

    friend class Genesis<ObjTypeId, Object::ptr(*)()>;

    static void static_init() {
      static_constructor<&Penguin::static_init>::c.run();
      Penguin::Factory::add<Object, Penguin>();
    }

    graphics::Sprite::ptr sprite;
};
}   // namespace client