#pragma once

#include "object.h"
#include "client/penguin.h"

void creation_registry_test() {
    Object::ptr penguin = Object::Factory::create(client::Penguin::type);
    assert(penguin->getType() == client::Penguin::type);
}