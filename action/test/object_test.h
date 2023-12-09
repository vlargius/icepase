#pragma once

#include <memory>

#include "object.h"

class ObjectTest {
public:
    using ptr = std::shared_ptr<ObjectTest>;

    inline static ObjTypeId type = ObjTypeId::next();

    static void static_init() {
      static_constructor<&ObjectTest::static_init>::c.run();
      Object::Factory::add<Object, ObjectTest>();
    }
};

void creation_registry_test() {
    ObjectTest::ptr test = Object::Factory::create(ObjectTest::type);
    assert(test->getType() == ObjectTest::type);
}