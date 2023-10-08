#pragma once

#include "genesis.h"

using RpcId = GenId<struct rpc_tag, uint16_t, 0>;
using RpcRegistry = Genesis<RpcId, void (*)(instream &)>;

namespace replication {

enum class Action : uint8_t { Create, Update, Destroy, RPC, Max };

class Header {
  public:
    Action action;
    NetId netId;
    ObjTypeId classId = ObjTypeId::invalid;
    RpcId rpcId = RpcId::invalid;

    template <class Stream> void serialize(Stream &stream) {
        stream.template serialize<(int) Action::Max>(action);
        if (action == Action::RPC) {
            stream.serialize(rpcId);
        } else {
            stream.serialize(netId);
            if (action != Action::Destroy) {
                stream.serialize(classId);
            }
        }
    }

  private:
};
};   // namespace replication
