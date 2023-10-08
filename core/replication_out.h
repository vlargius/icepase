#pragma once

#include <unordered_map>
#include <vector>

#include "linker.h"
#include "outstream.h"
#include "replication_header.h"

namespace replication {

class Output {
  public:
    struct Command {
        Action action;
        ObjFields fields = 0;

        Command() = default;
        Command(ObjFields fields_) : action(Action::Create), fields(fields_) {}
        bool hasUpdate() const { return action == Action::Destroy || fields != 0; }

        void add(ObjFields fields_) { fields |= fields_; }
        void remove(ObjFields fields_) {
            fields &= ~fields_;
            if (action == Action::Destroy)
                action = Action::Update;
        }
    };

    void create(NetId net_id, ObjFields fields) { netId_command[net_id] = Command(fields); }

    void destroy(NetId net_id) { netId_command[net_id].action = Action::Destroy; }

    void markField(NetId net_id, ObjFields fields) { netId_command[net_id].add(fields); }

    void remove(NetId net_id) { netId_command.erase(net_id); }

    template <class Rpc, class... Args> void call(outstream &stream, const Args &...args) {
        Header{Action::RPC, NetId::invalid, ObjTypeId::invalid, Rpc::type}.serialize(stream);
        Rpc{args...}.serialize(stream);
    }

    void process(outstream &stream, const Linker &linker) {
        for (auto &pair : netId_command) {
            if (pair.second.hasUpdate()) {
                Command &command = pair.second;
                const NetId netId = pair.first;
                if (command.action == Action::Destroy) {
                    Header{command.action, netId}.serialize(stream);
                    command.remove(command.fields);
                    removeIds.emplace_back(netId);
                } else {
                    Object::ptr object = linker.get(netId);
                    Header{command.action, netId, object->getType()}.serialize(stream);
                    command.remove(object->write(stream));
                }
            }
        }

        for (const NetId id : removeIds) {
            netId_command.erase(id);
        }
        netId_command.clear();
    }

  private:
    std::unordered_map<NetId, Command> netId_command;
    std::vector<NetId> removeIds;
};
}   // namespace replication