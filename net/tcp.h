#pragma once

#include <memory>
#include <vector>

#include "i_socket.h"


namespace net {

class tcp : public ISocket<tcp> {
  public:

    void set_no_delay(bool is_no_delay) {
        int optval = !is_no_delay;
        if (setsockopt(rawSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof optval) < 0)
            report("tcp::set_no_delay");
    }

    bool listen(int backlog = SOMAXCONN) {

        if (::listen(rawSocket, backlog) < 0) {
            report("tcp::listen");
            return false;
        }
        return true;
    }

    ptr accept(address &from) {
        socklen_t length = from.size();
        socket_type newSocket = ::accept(rawSocket, from.raw(), &length);
        if (newSocket != INVALID_SOCKET) {
            return ptr(new tcp(newSocket));
        }

        report("tcp::accept");
        return nullptr;
    }

    friend ptr make_tcp(int);

    friend fd_set* fill(const std::set<tcp::ptr>*, fd_set&);
    friend void fill(const std::set<tcp::ptr>*, const fd_set &, std::set<tcp::ptr> *);

  private:

    using ISocket::ISocket;
};
}   // namespace net
