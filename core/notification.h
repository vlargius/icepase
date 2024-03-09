#pragma once

#include <deque>

#include "ack_range.h"
#include "identifier.h"
#include "instream.h"
#include "outstream.h"
#include "tagged_packet.h"
#include "timer.h"

namespace delivery {

class Notification {
  public:
    Notification(bool is_sending, bool is_getting) :
        isSendingAcks(is_sending), isGettingAcks(is_getting) {}
    inline TaggedPacket *write(outstream &stream);
    inline bool read(instream &stream);
    inline void processTimedOut();
    const std::deque<TaggedPacket>& getPackets() const { return packets; };

  private:
    static inline float acknowledgeTimeout = 5;//0.5f;
    std::deque<TaggedPacket> packets;
    Tag nextTag = 0;
    Tag expectedTag = 0;
    size_t dropppedCount = 0;
    size_t sentCount = 0;
    size_t deliveredCount = 0;
    bool isSendingAcks = false;
    bool isGettingAcks = false;

    std::deque<AckRange> pengingAcks;

    inline TaggedPacket *writeTag(outstream &stream);
    inline void writePendingAcks(outstream &stream);
    inline bool processTag(instream &stream);
    inline void addPending(Tag tag);
    inline void processAcks(instream &stream);
    inline void packetFailure(const TaggedPacket &packet);
    inline void packetSuccess(const TaggedPacket &packet);
};

inline TaggedPacket *Notification::write(outstream &stream) {
    TaggedPacket *out = writeTag(stream);
    writePendingAcks(stream);
    return out;
}

inline bool Notification::read(instream &stream) {
    bool out = processTag(stream);
    processAcks(stream);
    return out;
}

inline void Notification::processTimedOut() {
    const float timeoutStamp = timing::current() - acknowledgeTimeout;
    while (!packets.empty()) {
        const auto &packet = packets.front();
        if (packet.time < timeoutStamp) {
            packetFailure(packet);
            packets.pop_front();
        } else {
            break;
        }
    }
}

inline TaggedPacket *Notification::writeTag(outstream &stream) {
    Tag currentTag = nextTag++;
    stream.write(currentTag);
    ++sentCount;
    if (isGettingAcks) {
        packets.emplace_back(currentTag);
        return &packets.back();
    } else {
        return nullptr;
    }
}

inline void Notification::writePendingAcks(outstream &stream) {
    if (!isSendingAcks)
        return;

    const bool hasAcks = !pengingAcks.empty();
    stream.write(hasAcks);
    if (hasAcks) {
        pengingAcks.front().write(stream);
        pengingAcks.pop_front();
    }
}

inline bool Notification::processTag(instream &stream) {
    Tag readTag;
    stream.read(readTag);
    if (readTag < expectedTag && !TaggedPacket::is_wrapped(readTag, expectedTag))
        return false;

    expectedTag = readTag + 1;
    if (isSendingAcks)
        addPending(readTag);
    return true;
}

inline void Notification::addPending(Tag tag) {
    if (pengingAcks.empty() || !pengingAcks.back().extend(tag)) {
        pengingAcks.emplace_back(tag);
    }
}

inline void Notification::processAcks(instream &stream) {
    if (!isGettingAcks)
      return;

    bool hasAcks;
    stream.read(hasAcks);
    if (hasAcks) {
        AckRange range;
        range.read(stream);
        Tag nextAck = range.start;
        const Tag afterAck = range.start + range.count;
        while ((nextAck < afterAck || TaggedPacket::is_wrapped(nextAck, afterAck)) && !packets.empty()) {
            const TaggedPacket &packet = packets.front();
            if (packet.tag < nextAck) {
                auto packetCopy = packet;
                packets.pop_front();
                packetFailure(packetCopy);
            } else if (packet.tag == nextAck) {
                packetSuccess(packet);
                packets.pop_front();
                ++nextAck;
            } else if (packet.tag > nextAck) {
                nextAck = packet.tag;
            }
        }
    }
}

void Notification::packetFailure(const TaggedPacket &packet) {
    packet.data->fail(this);
    ++dropppedCount;
}

void Notification::packetSuccess(const TaggedPacket &packet) {
    packet.data->success();
    ++deliveredCount;
}
}   // namespace delivery