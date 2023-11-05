#pragma once

#include <string>

#include "address.h"
#include "input.h"
#include "instream.h"
#include "network_base.h"
#include "packet.h"
#include "replication_in.h"
#include "timer.h"
#include "user.h"

namespace client {
class Network final : public BaseNetwork {
  public:
    enum class State : uint8_t {
        Uninitialized,
        Joining,
        Joined,
    };

    static Network &get() {
        static Network instance;
        return instance;
    }

    replication::Input replication;

    template <State test_state> bool is() const { return state == test_state; }

    UserId getUserId() const { return userId; }

    void init(const net::address &server_address, const std::string &name_) {
        assert(state == State::Uninitialized);
        serverAddress = server_address;
        name = name_;

        ownAddress = net::address(net::AnyAddress, 0);
        BaseNetwork::init(ownAddress);
        state = State::Joining;
    }

    void processOutput() {
        switch (state) {
        case State::Joining: {
            sendJoinRequest();
            break;
        }
        case State::Joined: {
            sendUserInput();
            break;
        }

        default:
            break;
        }
    }

  private:
    State state = State::Uninitialized;
    net::address serverAddress;
    net::address ownAddress;
    std::string name = "default_client";
    UserId userId;
    Timer joiningTimer{2.f};

    Network() = default;

    void process(instream &stream, const net::address &address) override {
        PacketType packetType;
        stream.read<(int) PacketType::Max>(packetType);
        switch (packetType) {
        case PacketType::JoinResponse: {
            processConnect(stream);
            break;
        }
        case PacketType::Replication: {
            processReplication(stream);
            break;
        }
        case PacketType::Disconnect: {
            break;
        }
        default: {
            assert("unhandled packet type" && false);
            break;
        }
        }
    }

    void sendJoinRequest() {
        if (joiningTimer.elapsed()) {
            outstream connectionRequestPacket;
            connectionRequestPacket.write<(int) PacketType::Max>(PacketType::JoinRequest);
            connectionRequestPacket.write(name);
            send(connectionRequestPacket, serverAddress);
            joiningTimer.reset();
        }
    }

    void sendUserInput() {
        MoveList &moves = Input::get().moveList;
        if (!moves.empty()) {
            outstream inputPacket;
            inputPacket.write<(int) PacketType::Max>(PacketType::Input);
            const size_t count = moves.size();
            const size_t start = count > 3 ? count - 3 - 1 : 0;
            inputPacket.write<2>(count - start);
            for (int i = start; i < count; ++i) {
                moves[i].serialize(inputPacket);
            }

            send(inputPacket, serverAddress);
            moves.clear();
        }
    }

    void processConnect(instream &stream) {
        if (state == State::Joining) {
            stream.read(userId);
            std::cout << "'" << name << "' connected with id " << userId << std::endl;
            state = State::Joined;
        }
    }

    void processReplication(instream &stream) {
        if (state != State::Joined)
            return;

        bool isMoveProcessed;
        stream.read(isMoveProcessed);
        if (isMoveProcessed) {
            float lastTimestamp;
            stream.read(lastTimestamp);
            Input::get().moveList.remove(lastTimestamp);
        }

        replication.process(stream, linker);
    }
};
}   // namespace client