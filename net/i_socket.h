#pragma once

#include "netutils.h"

namespace net {

template <class T> class ISocket {
  public:
    using ptr = std::shared_ptr<T>;

    ~ISocket() { net::closesocket(rawSocket); }

    bool set_blocking_mode(bool is_blocking) {
        isBlocking = is_blocking;
        return set_blocking(rawSocket, is_blocking);
    }

    bool bind(const address &address) {
        if (::bind(rawSocket, address.raw(), address.size()) != 0) {
            report("socket::bind");
            return false;
        }
        return true;
    }

    bool connect(const address &to) {
        if (::connect(rawSocket, to.raw(), to.size()) < 0) {
            report("socket::connect");
            return false;
        }
        return true;
    }

    int send(const void *data, int length) {
        const int sentCount = ::send(rawSocket, static_cast<const char *>(data), length, 0);
        if (sentCount >= 0)
            return sentCount;
        report("socket::send");
        return -1;
    }

    int recieve(void *data, int length) {
        const int gotCount = ::recv(rawSocket, static_cast<char *>(data), length, 0);
        if (gotCount >= 0)
            return gotCount;
        // report("socket::recieve");
        return -1;
    }

  protected:
    ISocket(socket_type socket_) : rawSocket(socket_) {}

    socket_type rawSocket;
    bool isBlocking = true;
};
}   // namespace net
