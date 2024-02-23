#pragma once

#include <memory>
#include <object.h>
#include <math/point.h>
#include <user.h>
#include <graphics/renderer.h>

#include "linker.h"

class Penguin : public Object {
  public:
    using ptr = std::shared_ptr<Penguin>;
    inline static ObjTypeId type = ObjTypeId::next();
    enum Fields { Position = 1 << 0, All = Position };

    Point2 position;
    Point2 target;
    UserId userId;
    NetId netId;

    Penguin();

    ObjTypeId getType() const override { return type; }
    ObjFields getFields() const override { return Fields::All; }

    ObjFields write(outstream &stream, const ObjFields fields) override;

    void update() override;

    void read(instream &stream) override;

  private:
    graphics::Sprite::ptr sprite;

    static void static_init();
    static Object::ptr base_create() { return create(); }
    static Penguin::ptr create();

    friend class Genesis<ObjTypeId, Object::ptr (*)()>;
};