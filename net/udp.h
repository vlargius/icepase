#pragma once

#include <memory>

#include "i_socket.h"


namespace net {

class udp : public ISocket<udp> {
  public:

    void set_no_delay(bool is_no_delay) {
        int optval = !is_no_delay;
        if (setsockopt(rawSocket, IPPROTO_UDP, TCP_NODELAY, (const char*)&optval, sizeof optval) < 0)
            report("udp::set_no_delay");
    }

    int send_to(const void *data, int length, const address &address) {
        const int sentCount =
            ::sendto(rawSocket, static_cast<const char *>(data), length, 0, address.raw(), address.size());
        if (sentCount >= 0)
            return sentCount;

        report("udp::send_to");
        return -1;
    }

    int recieve_from(void *data, int length, address &address) {
        socklen_t addressLength = address.size();
        const int gotCount =
            ::recvfrom(rawSocket, static_cast<char *>(data), length, 0, address.raw(), &addressLength);
        if (gotCount >= 0)
            return gotCount;
        else if (!isBlocking)
            return 0;
        report("udp::recieve_from");
        return -1;
    }

    friend ptr make_udp(int family);

  private:
    using ISocket::ISocket;
};
}   // namespace net
