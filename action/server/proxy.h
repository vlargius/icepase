#pragma once

#include <string>

#include "address.h"
#include "client.h"
#include "replication_out.h"
#include "move_list.h"
#include "timer.h"


class Proxy {
  public:
    using ptr = std::shared_ptr<Proxy>;

    MoveList unprocessedMoves;
    replication::Output replication;
    bool isNeedReplication = false;

    Proxy(const net::address &address_, const std::string &name_, PlayerId player_id)
        : address(address_), name(name_), playerId(player_id) {}

    const PlayerId getId() const { return playerId; }
    const std::string &getName() const { return name; }
    const net::address &getAddress() const { return address; }
    float getLastPacketTime() const { return lastPacketTime; }

    void updateLastPacketTime() {
      lastPacketTime = timing::current();
    }

  private:
    net::address address;
    std::string name;
    PlayerId playerId;
    float lastPacketTime = -1.f;
};
