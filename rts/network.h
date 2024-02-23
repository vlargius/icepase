#pragma once

#include <limits>
#include <network_base.h>
#include <map>
#include <unordered_map>
#include <user.h>
#include <world.h>
#include <sstream>

#include "input.h"
#include "penguin.h"
#include "rand_number.h"
#include "turn_data.h"

static const int subTurnsPerTurn = 3;


class Network : public BaseNetwork {
  public:
    static Network &get() {
        static Network instance;
        return instance;
    }

    using BaseNetwork::linker;

    enum class State : uint8_t { Uninitialized, Unconnected, Lobby, Staring, Playing, Delay } state;

    enum class PacketType : uint8_t { Request, Welcome, NotMaster, CantConnect, Intoduce, Turn, Start, Count };

    UserId getUserId() const { return userId; }
    bool isDelayed() const { return state == State::Delay; }

    bool initMaster(const net::address &address_, const std::string &name_) {

        if (!init(address_))
            return false;

        isMaster = true;
        playerCount = 1;
        userId = UserId::next();
        state = State::Lobby;
        name = name_;

        user_name.emplace(userId, name);

        std::cout << "master '" << name << "' inited" << std::endl;

        return true;
    }

    bool initPeer(const net::address &address_, const std::string &name_) {
        const net::address address = net::address(net::AnyAddress, 0);
        if (!init(address))
            return false;

        state = State::Unconnected;
        masterAddress = address_;
        name = name_;

        std::cout << "client '" << name << "' inited" << std::endl;

        return true;
    }

    void processOutput() {
        switch (state) {
        case State::Unconnected: {
            updateConnect(false);
            break;
        }
        case State::Staring: {
            updateStarting();
            break;
        }
        case State::Playing: {
            updateSendTurnPacket();
            break;
        }
        default:
            break;
        }
    }

    void tryStartGame() {
        if (state < State::Staring && isMaster) {
            std::cout << "master starting the game" << std::endl;

            const uint32_t seed = RandNumber::instance->get<uint32_t>(0, std::numeric_limits<uint32_t>::max);
            RandNumber::instance->seed(seed);

            outstream outPacket;
            outPacket.write<(int) PacketType::Count>(PacketType::Start);
            outPacket.write(seed);

            for (auto &[address, user] : address_user) {
                send(outPacket, address);
            }
            startTimer.reset();
            state = State::Staring;
        }
    }

  private:
    using UsersTurnData = std::unordered_map<UserId, TurnData>;

    static const size_t playerMaxCount = 4;
    bool isMaster = false;
    UserId userId;
    net::address masterAddress;
    std::string name;
    std::map<UserId, std::string> user_name;
    std::unordered_map<net::address, UserId> address_user;
    std::map<UserId, net::address> user_address;
    std::vector<UsersTurnData> turnHistory;
    size_t subTurnCounter = 0;
    int turnCounter = -2;

    size_t playerCount;

    Timer connectionTimer{3.f};
    Timer startTimer{3.f};

    Network() {
        turnHistory.resize(100000);
    }

    void connectionReset(const net::address &address) override {
        if (address_user.find(address) != address_user.end()) {
            std::cerr << "reset connection" << std::endl;

            UserId resetUserId = address_user[address];
            address_user.erase(address);
            user_address.erase(resetUserId);
            user_name.erase(resetUserId);

            playerCount--;

            if (address == masterAddress) {
                const UserId newMasterUserId = user_name.begin()->first;
                if (newMasterUserId == userId) {
                    isMaster = true;
                } else {
                    masterAddress = user_address[newMasterUserId];
                }
            }

            if (state == State::Delay) {
                tryNextTurn();
            }
        }
    }

    void updateConnect(bool is_force) {
        if (is_force || connectionTimer.elapsed()) {
            outstream connectPacket;
            connectPacket.serialize<(int) PacketType::Count>(PacketType::Request);
            connectPacket.write(name);
            send(connectPacket, masterAddress);

            connectionTimer.reset();
        }
    }

    void updateStarting() {
        if (startTimer.elapsed()) {
            state = State::Playing;

            for (auto &[playerId, playerName] : user_name) {
                Penguin::ptr penguin = World::get().add<Penguin>();
                penguin->userId = playerId;
                penguin->position.x = RandNumber::instance->getFloat();
                penguin->position.y = RandNumber::instance->getFloat();
                penguin->target.x = penguin->position.x;
                penguin->target.y = penguin->position.y;
            }
        }
    }

    void updateSendTurnPacket() {
        ++subTurnCounter;
        if (subTurnCounter == subTurnsPerTurn) {
            TurnData data{
                RandNumber::instance->get<uint32_t>(0, std::numeric_limits<size_t>::max),
                (uint32_t) -1,   // ComputeGlobalCRC(),
                Input::get().commandList
            };
            outstream outPacket;
            outPacket.write<int(PacketType::Count)>(PacketType::Turn);
            outPacket.write(turnCounter + 2);
            outPacket.write(userId);
            data.write(outPacket);

            for (auto [user, address] : user_address) {
                send(outPacket, address);
            }

            turnHistory[turnCounter + 2].emplace(userId, data);
            Input::get().clear();

            if (turnCounter >= 0) {
                tryNextTurn();
            } else {
                ++turnCounter;
                subTurnCounter = 0;
            }
        }
    }

    void process(instream &stream, const net::address &address) override {
        switch (state) {
        case State::Unconnected: {
            processConnect(stream, address);
            break;
        }
        case State::Lobby: {
            processLobby(stream, address);
            break;
        }
        case State::Playing: {
            processPlaying(stream, address);
            break;
        }
        case State::Delay: {
            processDelay(stream, address);
            break;
        }
        default:
            break;
        }
    }

    void processConnect(instream &stream, const net::address &address) {
        PacketType packetType;
        stream.read<(int) PacketType::Count>(packetType);
        switch (packetType) {
        case PacketType::NotMaster: {
            assert("NotMaster" && false);
            // stream.read(masterAddress);
            // updateConnect(true);
            break;
        }
        case PacketType::Welcome: {
            processWelcome(stream, address);
            break;
        }
        case PacketType::CantConnect:
        default:
            assert("cant connect " && false);
            break;
        }
    }

    void processWelcome(instream &stream, const net::address &address) {
        stream.read(userId);
        user_name.emplace(userId, name);

        masterAddress = address;

        UserId masterUserId;
        stream.read(masterUserId);
        user_address.emplace(masterUserId, address);
        address_user.emplace(address, masterUserId);

        stream.read(playerCount);
        assert(playerCount > 0);
        net::address::ptr readAddress;
        for (int i = 0; i < playerCount - 1; ++i) {
            UserId readUserId;
            stream.read(readUserId);
            std::string addressStr;
            stream.read(addressStr);
            readAddress = net::make_address(addressStr);
            user_address.emplace(readUserId, *readAddress);
            address_user.emplace(*readAddress, readUserId);
        }

        for (int i = 0; i < playerCount; ++i) {
            UserId readUserId;
            std::string readName;
            stream.read(readUserId);
            stream.read(readName);
            user_name.emplace(readUserId, readName);
        }

        ++playerCount;

        outstream outPacket;
        outPacket.write<(int) PacketType::Count>(PacketType::Intoduce);
        outPacket.write(userId);
        outPacket.write(name);
        for (const auto &[id, address] : user_address) {
            send(outPacket, address);
        }

        std::cout << "switch to lobby" << std::endl;

        state = State::Lobby;
    }

    void processLobby(instream &stream, const net::address &address) {
        PacketType packetType;
        stream.read<(int) PacketType::Count>(packetType);
        switch (packetType) {
        case PacketType::Request: {
            processConnectionRequest(stream, address);
            break;
        }
        case PacketType::Intoduce: {
            processIntroduction(stream, address);
            break;
        }
        case PacketType::Start: {
            processStart(stream, address);
            break;
        }
        default:
            break;
        }
    }

    void processConnectionRequest(instream &stream, const net::address &address) {
        if (address_user.find(address) != address_user.end()) {
            return;
        }

        if (playerCount > playerMaxCount) {
            outstream outPacket;
            outPacket.write<(int) PacketType::Count>(PacketType::CantConnect);
            send(outPacket, address);
            return;
        }

        if (isMaster) {
            std::string readName;
            stream.read(readName);
            outstream outPacket;
            outPacket.write<(int) PacketType::Count>(PacketType::Welcome);

            UserId newUserId = UserId::next();
            outPacket.write(newUserId);
            outPacket.write(userId);
            outPacket.write(playerCount);

            for (const auto &[playerId, playerAddress] : user_address) {
                outPacket.write(playerId);
                std::stringstream ss;
                ss << playerAddress;
                std::string addressStr;
                ss >> addressStr;
                outPacket.write(addressStr);
            }

            for (const auto &[playerId, playerName] : user_name) {
                outPacket.write(playerId);
                outPacket.write(playerName);
            }

            send(outPacket, address);

            ++playerCount;
            address_user.emplace(address, newUserId);
            user_address.emplace(newUserId, address);
            user_name.emplace(newUserId, readName);

            std::cout << "connect '" << readName << "' id: " << newUserId << " address: " << address << std::endl;
        } else {
            outstream outPacket;
            outPacket.write<(int) PacketType::Count>(PacketType::NotMaster);
            std::stringstream ss;
            ss << masterAddress;
            std::string addressStr;
            ss >> addressStr;
            outPacket.write(addressStr);
            send(outPacket, address);
        }
    }

    void processIntroduction(instream &stream, const net::address &address) {
        if (!isMaster) {
            UserId readUserId;
            std::string readName;

            stream.read(readUserId);
            stream.read(readName);

            std::cout << "introduce " << readName << " " << address << std::endl;

            ++playerCount;
            user_address.emplace(readUserId, address);
            address_user.emplace(address, readUserId);
            user_name.emplace(readUserId, readName);
        }
    }

    void processStart(instream &stream, const net::address &address) {
        if (address != masterAddress)
            return;
        std::cout << "switch to starting" << std::endl;

        uint32_t seed;
        stream.read(seed);
        RandNumber::instance->seed(seed);

        state = State::Staring;
        startTimer.reset();
    }

    void processPlaying(instream &stream, const net::address &address) {
        PacketType packetType;
        stream.read<int(PacketType::Count)>(packetType);
        if (packetType == PacketType::Turn) {
            handleTurnPacket(stream, address);
        }
    }

    void processDelay(instream &stream, const net::address &address) {
        PacketType packetType;
        stream.read<int(PacketType::Count)>(packetType);
        if (packetType == PacketType::Turn) {
            handleTurnPacket(stream, address);
            tryNextTurn();
        }
    }

    bool checkSynch(UsersTurnData &) { return true; /*todo implement checking synch*/ }

    void tryNextTurn() {
        if (turnHistory[turnCounter + 1].size() == playerCount) {
            if (isDelayed()) {
                Input::get().clear();
                state = State::Playing;
            }

            ++turnCounter;
            subTurnCounter = 0;
            // std::cout << "current turn " << turnCounter << std::endl;

            if (checkSynch(turnHistory[turnCounter])) {
                for (auto& [playerId, commands] : turnHistory[turnCounter]) {
                    commands.commandList.process(playerId);
                }

            } else {
                // Engine::isRunning = false; stop the execution
            }
        } else {
            state = State::Delay;
            std::cout << "delay: wait for the next turn" << std::endl;
        }
    }

    void handleTurnPacket(instream &stream, const net::address &address) {
        auto it = address_user.find(address);
        if (it != address_user.end()) {
            const UserId expectedId = it->second;

            int readTurnCounter;
            UserId readId;
            stream.read(readTurnCounter);
            stream.read(readId);

            if (readId != expectedId) {
                std::cerr << "turn data from a wrong player id - " << expectedId << ", got - " << readId << std::endl;
                return;
            }

            TurnData data;
            data.read(stream);

            turnHistory[readTurnCounter].emplace(readId, data);
        }
        else
        {
            std::cerr << "cant find address: " << address << " " << std::hash<net::address>{}(address) << std::endl;
            for (auto& [addr, user] : address_user)
                std::cerr << user << " " << addr << " " << std::hash<net::address>{}(addr) << std::endl;
        }
    }
};