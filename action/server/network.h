#pragma once

#include <string>
#include <unordered_map>

#include "network_base.h"
#include "address.h"
#include "instream.h"
#include "outstream.h"
#include "packet.h"
#include "proxy.h"

namespace server {
class Network final : public BaseNetwork {
  public:
    static Network &get() {
        static Network instance;
        return instance;
    }

    using BaseNetwork::init;

    Proxy::ptr getClient(const PlayerId& player_id) {
        auto it = player_client.find(player_id);
        return it != player_client.end() ? it->second : nullptr;
    }

  private:
    std::unordered_map<net::address, Proxy::ptr> address_client;
    std::unordered_map<PlayerId, Proxy::ptr> player_client;

    Network() = default;

    void process(instream &stream, const net::address &address) override {
        auto it = address_client.find(address);
        if (it == address_client.end()) {
            handleNewClient(stream, address);
        } else {
            process(stream, it->second);
        }
    }

    void sendConnectResponse(const Proxy::ptr proxy) const {
        outstream approvePacket;
        approvePacket.write<(int) PacketType::Max>(PacketType::JoinResponse);
        approvePacket.write(proxy->getId());
        send(approvePacket, proxy->getAddress());
    }

    void handleNewClient(instream &stream, const net::address &address) {
        PacketType packetType;
        stream.read<(int) PacketType::Max>(packetType);
        if (packetType == PacketType::JoinRequest) {
            std::string name;
            stream.read(name);
            Proxy::ptr newClient = std::make_shared<Proxy>(address, name, PlayerId::next());
            address_client[address] = newClient;
            player_client[newClient->getId()] = newClient;
            sendConnectResponse(newClient);
        } else {
            assert("todo log not joining packet type from a new address");
        }
    }

    void processUserInput(instream &stream, Proxy::ptr proxy) {
        size_t moveCount = 0;
        stream.read<2>(moveCount);

        Move move;
        for (; moveCount > 0; --moveCount) {
            move.serialize(stream);
            proxy->unprocessedMoves.add(move);
            proxy->isNeedReplication = true;
        }
    }

    void process(instream &stream, Proxy::ptr proxy) {
        proxy->updateLastPacketTime();

        PacketType packetType;
        stream.read<(int) PacketType::Max>(packetType);
        switch (packetType) {
        case PacketType::JoinRequest: {
            sendConnectResponse(proxy);
            break;
        }
        case PacketType::Input: {
            processUserInput(stream, proxy);
            break;
        }
        default:
            assert("process incoming packet on server. undefined type");
        }
    }

    void processOutput() {
        for (const auto &it : address_client) {
            if (it.second->isNeedReplication) {
                processOutput(it.second);
            }
        }
    }

    void processOutput(Proxy::ptr proxy) {
        outstream replicationPacket;
        replicationPacket.write<(int)PacketType::Max>(PacketType::Replication);

        replicationPacket.write(proxy->isNeedReplication);
        if (proxy->isNeedReplication) {
            replicationPacket.write(proxy->unprocessedMoves.getLastTimestamp());
            proxy->isNeedReplication = false;
        }

        proxy->replication.process(replicationPacket, linker);
        send(replicationPacket, proxy->getAddress());
    }
};
}   // namespace server