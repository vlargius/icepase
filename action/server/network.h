#pragma once

#include <string>
#include <unordered_map>

#include "address.h"
#include "instream.h"
#include "network_base.h"
#include "outstream.h"
#include "packet.h"
#include "proxy.h"
#include "timer.h"
#include "replication_transmission.h"

namespace server {
class Network final : public BaseNetwork {
  public:
    static Network &get() {
        static Network instance;
        return instance;
    }

    using BaseNetwork::init;

    Proxy::ptr getClient(const UserId &user_id) {
        auto it = userId_client.find(user_id);
        return it != userId_client.end() ? it->second : nullptr;
    }

    NetId link(Object::ptr object) {
        const NetId netId = linker.get(object, true);
        for (const auto &[userId, client] : userId_client) {
            client->replication.create(netId, object->getFields());
        }
        return netId;
    }

    void unlink(Object::ptr object) {
        if (const NetId netId = linker.get(object)) {
            for (const auto &[userId, client] : userId_client) {
                client->replication.destroy(netId);
            }
        }
    }

    void processOutput() {
        for (const auto &[address, client] : address_client) {
            client->notification.processTimedOut();
            if (client->isMoveProcessed) {
                processOutput(client);
            }
        }
    }

    void updateFields(NetId net_id, ObjFields fields) {
        for (const auto &[userId, client] : userId_client) {
            client->replication.markField(net_id, fields);
        }
    }

    void processDisconnect() {
        std::vector<Proxy::ptr> disconnectedClients;
        const float disconnectAtTime = timing::current() - disconnectTimeout;
        for (const auto &[userId, client] : userId_client) {
            if (client->getLastPacketTime() < disconnectAtTime) {
                disconnectedClients.push_back(client);
            }
        }
        for (Proxy::ptr client : disconnectedClients)
            processDisconnect(client);
    }

    void respawn() {
        for (auto &[userId, client] : userId_client) {
            client->respawn(false);
        }
    }

  private:
    std::unordered_map<net::address, Proxy::ptr> address_client;
    std::unordered_map<UserId, Proxy::ptr> userId_client;
    const float disconnectTimeout = 10.f;

    Network() = default;

    void process(instream &stream, const net::address &address) override {
        auto it = address_client.find(address);
        if (it == address_client.end()) {
            processNewClient(stream, address);
        } else {
            process(stream, it->second);
        }
    }

    void connectionReset(const net::address &address) override {
        auto it = address_client.find(address);
        if (it != address_client.end())
            processDisconnect(it->second);
    };

    void sendConnectResponse(const Proxy::ptr proxy) const {
        outstream approvePacket;
        approvePacket.write<(int) PacketType::Max>(PacketType::JoinResponse);
        approvePacket.write(proxy->getId());
        send(approvePacket, proxy->getAddress());
    }

    void processNewClient(instream &stream, const net::address &address) {
        PacketType packetType;
        stream.read<(int) PacketType::Max>(packetType);
        if (packetType == PacketType::JoinRequest) {
            std::string name;
            stream.read(name);
            Proxy::ptr newClient = std::make_shared<Proxy>(address, name, UserId::next());
            address_client[address] = newClient;
            userId_client[newClient->getId()] = newClient;
            newClient->respawn(true);

            sendConnectResponse(newClient);
            for (const auto &[netId, object] : linker.getNetIdObject()) {
                newClient->replication.create(netId, object->getFields());
            }

            std::cout << "'" << name << "' connected with id " << newClient->getId() << std::endl;
        } else {
            assert("todo log not joining packet type from a new address" && false);
        }
    }

    void processUserInput(instream &stream, Proxy::ptr proxy) {
        size_t moveCount = 0;
        stream.read<2>(moveCount);

        Move move;
        for (; moveCount > 0; --moveCount) {
            move.serialize(stream);
            if (proxy->unprocessedMoves.add(move))
                proxy->isMoveProcessed = true;
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
            if (proxy->notification.read(stream)) {
                processUserInput(stream, proxy);
            }
            break;
        }
        default:
            assert("process incoming packet on server. undefined type" && false);
        }
    }

    void processOutput(Proxy::ptr proxy) {
        outstream replicationPacket;
        replicationPacket.write<(int) PacketType::Max>(PacketType::Replication);

        delivery::TaggedPacket *packet = proxy->notification.write(replicationPacket);

        replicationPacket.write(proxy->isMoveProcessed);
        if (proxy->isMoveProcessed) {
            replicationPacket.write(proxy->unprocessedMoves.getLastTimestamp());
            proxy->isMoveProcessed = false;
        }

        replication::TransmissionData *transmission = new replication::TransmissionData();
        proxy->replication.process(replicationPacket, linker, transmission);
        packet->data.reset(transmission);

        send(replicationPacket, proxy->getAddress());
    }

    void processDisconnect(Proxy::ptr client) {
        std::cout << "disconnect client '" << client->getName() << "' with id " << client->getId() << std::endl;
        userId_client.erase(client->getId());
        address_client.erase(client->getAddress());

        if (userId_client.empty()) {
            assert("todo stop simulation" && false);
        }
    }
};
}   // namespace server