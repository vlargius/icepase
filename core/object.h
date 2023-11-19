#pragma once

#include <memory>

#include "identifier.h"
#include "instream.h"
#include "outstream.h"
#include "genesis.h"
#include "static_constructor.h"


using ObjTypeId = GenId<struct object_class_id, uint8_t, 0>;
using ObjFields = uint32_t;

class Object {
  public:
    using ptr = std::shared_ptr<Object>;
    inline static ObjTypeId type = ObjTypeId::next();
    using Factory = Genesis<ObjTypeId, Object::ptr(*)()>;

    bool isMarkedDestroyed = false;

    Object() = default;
    virtual ~Object() {};

    virtual ObjTypeId getType() const { return type; }
    virtual ObjFields getFields() const { return 1; }

    virtual void update() {}

    virtual void read(instream &stream) {}
    virtual ObjFields write(outstream &stream, const ObjFields fields) { return 0; }

  protected:
};
