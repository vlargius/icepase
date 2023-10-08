#pragma once

#include <list>
#include <queue>

#include "instream.h"
#include "linker.h"
#include "netutils.h"
#include "outstream.h"
#include "timer.h"

class BaseNetwork {
  public:
    Linker linker;

    void processInput() {
        readInputQueue();
        processInputQueue();
    }

  protected:


    bool init(const net::address &address) {
        socket = net::make_udp();
        if (!socket || !socket->bind(address) || socket->set_blocking_mode(false)) {
            return false;
        }
        return true;
    }

    net::udp::ptr socket;
    size_t maxPacketPerFrameCount = 10;

    class Packet {
      public:
        Packet(float timestamp_, const instream &buffer_, net::address address_)
            : timestamp(timestamp_), buffer(buffer_), address(address_) {}

        float getTimestamp() const { return timestamp; }
        instream &getBuffer() { return buffer; }
        net::address getAddress() const { return address; }

      private:
        float timestamp = -1;
        instream buffer;
        net::address address;
    };
    std::queue<Packet, std::list<Packet>> packets;

    BaseNetwork() = default;

    virtual void process(instream &stream, const net::address &address) = 0;

    void send(outstream &packet, const net::address &address) const {
        int sendByteCount = socket->send_to(packet.data(), packet.byte_size(), address);
    }

    void readInputQueue() {
        const size_t packetSize = 1500;
        char packetBuffer[packetSize];
        net::address fromAddress;

        size_t recievedPacketCount = 0;
        while (recievedPacketCount < maxPacketPerFrameCount) {
            int readByteCount = socket->recieve_from(packetBuffer, packetSize, fromAddress);
            if (readByteCount == 0) {
                break;
            } else if (readByteCount < 0) {
                connectionReset(fromAddress);
            } else {
                ++recievedPacketCount;
                packets.emplace(timing::current(), instream(packetBuffer, readByteCount * 8), fromAddress);
            }
        }
    }

    void connectionReset(const net::address &address) {}

    void processInputQueue() {
        while (!packets.empty()) {
            Packet &nextPacket = packets.front();
            if (timing::current() > nextPacket.getTimestamp()) {
                process(nextPacket.getBuffer(), nextPacket.getAddress());
                packets.pop();
            } else {
                break;
            }
        }
    }
};