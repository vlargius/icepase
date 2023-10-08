#pragma once

#include <memory>
#include <cstring>
#include "nettools.h"


namespace net {

const uint32_t AnyAddress = INADDR_ANY;

class address {
  public:
    using ptr = std::shared_ptr<address>;

    address() { ::memset(&rawAddress, 0, sizeof(sockaddr_in)); }

    address(uint32_t _address, uint16_t port, int family = AF_INET) {
        get_sockaddr_in()->sin_family = family;
        // get_sockaddr_in()->sin_addr.S_un.S_addr = htonl(_address);
        get_sockaddr_in()->sin_addr.s_addr = htonl(_address);
        get_sockaddr_in()->sin_port = htonl(port);
    }

    address(const sockaddr &_sockaddr) { ::memcpy(&rawAddress, &_sockaddr, sizeof(sockaddr)); }
    address(const sockaddr_in6 &_sockaddr) { ::memcpy(&rawAddress, &_sockaddr, sizeof(sockaddr_in6)); }

    int size() const { return sizeof(rawAddress); }
    const sockaddr* raw() const { return &(sockaddr&)rawAddress; }
    sockaddr* raw() { return &(sockaddr&)rawAddress; }
    const bool operator==(const address &other) const {
        return ::strcmp(other.raw()->sa_data, raw()->sa_data) == 0;
    }

  private:
    sockaddr_in6 rawAddress;

    sockaddr_in *get_sockaddr_in() { return reinterpret_cast<sockaddr_in *>(&rawAddress); }
};
}   // namespace net

namespace std {
template <> struct hash<net::address> {
    size_t operator()(const net::address &address_) const {
      return std::hash<const char*>{}(address_.raw()->sa_data);
    }
};
}   // namespace std
