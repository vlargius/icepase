#pragma once

#include <string>

#include "address.h"
#include "move_list.h"
#include "replication_out.h"
#include "timer.h"
#include "user.h"

class Proxy {
  public:
    using ptr = std::shared_ptr<Proxy>;

    MoveList unprocessedMoves;
    replication::Output replication;
    bool isMoveProcessed = false;

    Proxy(const net::address &address_, const std::string &name_, UserId user_id_)
        : address(address_), name(name_), userId(user_id_) {
        updateLastPacketTime();
    }

    const UserId getId() const { return userId; }
    const std::string &getName() const { return name; }
    const net::address &getAddress() const { return address; }
    float getLastPacketTime() const { return lastPacketTime; }

    void updateLastPacketTime() { lastPacketTime = timing::current(); }

  private:
    net::address address;
    std::string name;
    UserId userId;
    float lastPacketTime = -1.f;
};
