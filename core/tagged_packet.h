#pragma once

#include <stdint.h>

#include "timer.h"
#include "transmission_data.h"

namespace delivery {
using Tag = uint16_t;

class TaggedPacket {
  public:
    static bool is_wrapped(Tag from, Tag to) {
      const Tag band = 5;
      #undef max
      return from > std::numeric_limits<Tag>::max() - band && to < band;
    }
    TaggedPacket(Tag tag_) : tag(tag_), time(timing::current()) {}

    Tag tag = 0;
    float time = -1.f;
    TransmissionData::ptr data;
};
}   // namespace delivery