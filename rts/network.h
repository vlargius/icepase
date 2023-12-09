#pragma once

#include <network_base.h>
#include <user.h>
#include <limits>

#include "input.h"


static const int subTurnsPerTurn = 10;

class Network : public BaseNetwork {
  public:
    static Network &get() {
        static Network instance;
        return instance;
    }

    enum class State {
      Uninitialized,
      Unconnected,
      Lobby,
      Staring,
      Playing,
      Delay
    } state;

    UserId getUserId() const { return userId; }

    bool initPeer(uint16_t port, const std::string& name_) {
        net::address ownAddress{net::AnyAddress, port};
        if (!init(ownAddress))
            return false;

        isMaster = true;
        userId = UserId::next();
        state = State::Lobby;
        name = name_;

        return true;
    }

    bool initMaster(const net::address& master_address, const std::string& name_) {
        net::address ownAddress{net::AnyAddress, 0};
        if (!init(ownAddress))
            return false;

        state = State::Unconnected;
        masterAddress = master_address;
        name = name_;

        return true;
    }

    // void sendTurnPacket() {
    //     ++subTurnCounter;
    //     if (subTurnCounter == subTurnsPerTurn) {
    //         TurnData data {
    //             userId,
    //             RandGen::get().random(0, std::numeric_limits<size_t>::max),
    //             ComputeGlobalCRC(),
    //             Input::get().commandList
    //         };
    //         outstream packet;
    //         packet.write(turnCC);
    //         packet.write(turnCounter + 2);
    //         packet.write(userId);
    //         data.write(packet);

    //         for (auto [user, socket] : user_socket) {
    //             send(user, packet);
    //         }

    //         turnData[turnCounter + 2].emplace(userId, data);
    //         Input::get().clear();

    //         if (turnCounter > 0) {
    //             tryNextTurn();
    //         } else {
    //             ++turnCounter;
    //             subTurnCounter = 0;
    //         }
    //     }
    // }

    // void tryNextTurn() {
    //     if (turnData[turnCounter + 1].size() == playerCount) {
    //         if (state == State::Delay) {
    //             Input::get().clear();
    //             state = State::Working;
    //             SDL_Delay(100);
    //         }

    //         ++turnCounter;
    //         subTurnCounter = 0;

    //         if (cehckSynch(turnData[turnCounter])) {
    //             for (auto& [command, input] : turnData[turnCounter]) {
    //                 input.commands.process(command);
    //             }

    //         } else {
    //             // Engine::isRunning = false; stop the execution
    //         }
    //     } else {
    //         state = State::Delay;
    //     }
    // }

  private:
    bool isMaster = false;
    UserId userId;
    net::address masterAddress;
    std::string name;
    size_t subTurnCounter;
    size_t turnCC;
    size_t turnCounter;

    size_t playerCount;

    Network() = default;

    void process(instream &stream, const net::address &address) override {
        // todo
    }
};