#pragma once

#include <memory>

#include "identifier.h"
#include "instream.h"
#include "outstream.h"
#include "genesis.h"
#include "static_constructor.h"


using ObjId = GenId<struct object_tag, size_t, 0>;
using ObjTypeId = GenId<struct object_class_id, uint8_t, 0>;
using ObjFields = uint32_t;

class Object {
  public:
    using ptr = std::shared_ptr<Object>;
    using ClassId = ObjTypeId;
    static ObjTypeId type;
    using Factory = Genesis<ObjTypeId, Object::ptr(*)()>;

    static void static_init() {
      static const Object obj;
      static_constructor<&Object::static_init>::c();
      type = ObjTypeId::next();
      Object::Factory::add<Object>();
    }

    bool isMarkedDestroyed = false;

    Object() = default;
    Object(ObjId id_) : id(id_) {}

    virtual ObjTypeId getType() const { return type; }
    virtual ObjFields getFields() const { return 1; }
    ObjId getId() const { return id; }

    virtual void read(instream& stream) {
      stream.read(id);
    }

    virtual ObjFields write(outstream& stream) {
      stream.write(id);
      return 0;
    }

    static Object::ptr create() { return std::make_shared<Object>(); }
  private:
    ObjId id = ObjId::invalid;
};
ObjTypeId Object::type;

namespace std {
template <>
struct hash<Object> {
    size_t operator()(const Object &object) const noexcept { return std::hash<ObjId>{}(object.getId()); }
};

template <>
struct hash<Object::ptr> {
    size_t operator()(const Object::ptr &object) const noexcept { return std::hash<ObjId>{}(object->getId()); }
};
}   // namespace std
