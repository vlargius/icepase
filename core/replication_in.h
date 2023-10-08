#pragma once

#include <cassert>
#include <memory>

#include "replication_header.h"
#include "instream.h"
#include "object.h"
#include "linker.h"


namespace replication {
class Input {
  public:
    void process(instream &stream, Linker& linker) {
        while (!stream.empty()) {
            Header header;
            header.serialize(stream);
            switch (header.action) {
            case Action::Create: {
                Object::ptr object = Object::Factory::create(header.classId);
                linker.add(object, header.netId);
                object->read(stream);
                break;
            }
            case Action::Update: {
                if (Object::ptr object = linker.get(header.netId)) {
                    object->read(stream);
                } else {
                    object = Object::Factory::create(header.classId);
                    object->read(stream);
                }
                break;
            }
            case Action::Destroy: {
                Object::ptr obejct = linker.get(header.netId);
                obejct->isMarkedDestroyed = true;
                linker.remove(header.netId);
                break;
            }
            case Action::RPC: {
                RpcRegistry::create(header.rpcId, stream);
                break;
            }
            default:
                assert("unrecognized replication header action" && false);
                break;
            }
        }
    }

  private:
};
}   // namespace replication