#pragma once

#include <unordered_map>

#include "identifier.h"
#include "object.h"

using NetId = GenId<struct net_tag, size_t, 0>;

class Linker {
  public:
    const std::unordered_map<NetId, Object::ptr> &getNetIdObject() { return netId_object; }

    NetId get(Object::ptr object, bool create = false) {
        auto it = object_netId.find(object);
        if (it != object_netId.end()) {
            return it->second;
        } else if (create) {
            add(object, NetId::next());
            return object_netId[object];
        } else {
            return NetId::invalid;
        }
    }

    Object::ptr get(const NetId id) const {
        auto it = netId_object.find(id);
        return it == netId_object.end() ? nullptr : it->second;
    }

    void add(Object::ptr object, NetId netId) {
        netId_object[netId] = object;
        object_netId[object] = netId;
    }

    void remove(Object::ptr object) {
        NetId id = object_netId[object];
        netId_object.erase(id);
        object_netId.erase(object);
    }

    void remove(const NetId& id) {
        Object::ptr object = netId_object[id];
        netId_object.erase(id);
        object_netId.erase(object);
    }

  private:
    std::unordered_map<NetId, Object::ptr> netId_object;
    std::unordered_map<Object::ptr, NetId> object_netId;
};