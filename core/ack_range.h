#pragma once

#include "instream.h"
#include "outstream.h"
#include "tagged_packet.h"

namespace delivery {

struct AckRange {

    Tag start = 0;
    size_t count = 0;

    AckRange() = default;
    AckRange(Tag start_) : start(start_), count(1) {}

    bool extend(Tag tag) {
        if (tag == start + count) {
            ++count;
            return true;
        }
        return false;
    }

    void write(outstream &stream) const {
        stream.write(start);
        bool hasCount = count > 1;
        stream.write(hasCount);
        if (hasCount) {
            size_t countMinusOne = count - 1;
            uint8_t countToAck = countMinusOne > 255 ? 255 : static_cast<uint8_t>(countMinusOne);
            stream.write(countToAck);
        }
    }

    void read(instream &stream) {
        stream.read(start);
        bool hasCount;
        stream.read(hasCount);
        if (hasCount) {
            uint8_t countMinusOne;
            stream.read(countMinusOne);
            count = countMinusOne + 1;
        } else {
            count = 1;
        }
    }
};

}   // namespace delivery