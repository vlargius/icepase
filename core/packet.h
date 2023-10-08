#pragma once

#include <stdint.h>

enum class PacketType : uint8_t {
    JoinRequest,
    JoinResponse,
    Replication,
    Input,
    Disconnect,
    Max
};