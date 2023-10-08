#pragma once

#include <memory>
#include <set>

#include "address.h"
#include "tcp.h"
#include "udp.h"


#ifdef _WIN
#pragma comment(lib, "Ws2_32.lib")

namespace net {
namespace {
struct Winsock2Initializer {
    Winsock2Initializer() {
        WORD wVersionRequested{MAKEWORD(2, 2)};
        WSADATA wsaData;
        if (int error = WSAStartup(wVersionRequested, &wsaData)) {
            report("", error);
            return;
        }
    }
    ~Winsock2Initializer() { WSACleanup(); }

} static const initializer;
}
}
#endif

namespace net {

address::ptr make_address(const std::string &address_str, int family = AF_INET) {
    const std::string &defaultService = "1111";

    auto separator = address_str.find_last_of(':');
    const std::string host = address_str.substr(0, separator);
    const std::string service = separator == std::string::npos ? defaultService : address_str.substr(separator + 1);

    addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = family;

    addrinfo *result;
    if (getaddrinfo(host.c_str(), service.c_str(), &hint, &result)) {
        report("make_address(" + address_str +")");
        if (result) {
            freeaddrinfo(result);
        }
        return nullptr;
    }

    while (!result->ai_addr && result->ai_next) {
        result = result->ai_next;
    }

    if (!result->ai_addr) {
        freeaddrinfo(result);
        return nullptr;
    }
    auto out = family == AF_INET
        ? std::make_shared<net::address>(*result->ai_addr)
        : std::make_shared<net::address>((sockaddr_in6&)*result->ai_addr);
    freeaddrinfo(result);
    return out;
}

udp::ptr make_udp(int family = AF_INET) {
    socket_type rawSocket = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
    int optval = 1;
    setsockopt(rawSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);
    if (rawSocket != INVALID_SOCKET)
        return udp::ptr(new udp(rawSocket));
    report("net::make_udp");
    return nullptr;
}

tcp::ptr make_tcp(int family = AF_INET) {
    socket_type rawSocket = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    int optval = 1;
    setsockopt(rawSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);
    if (rawSocket != INVALID_SOCKET)
        return tcp::ptr(new tcp(rawSocket));
    report("net::make_tcp");
    return nullptr;
}

fd_set *fill(const std::set<tcp::ptr> *sockets, fd_set &out) {
    if (sockets) {
        FD_ZERO(&out);
        for (const tcp::ptr &socket : *sockets) {
            FD_SET(socket->rawSocket, &out);
        }
        return &out;
    } else {
        return nullptr;
    }
}

void fill(const std::set<tcp::ptr> *sockets, const fd_set &set, std::set<tcp::ptr> *out) {
    if (sockets && out) {
        out->clear();
        for (const tcp::ptr &socket : *sockets) {
            if (FD_ISSET(socket->rawSocket, &set)) {
                out->insert(socket);
            }
        }
    }
}

int select(const std::set<tcp::ptr> *in_read, std::set<tcp::ptr> *out_read,
    const std::set<tcp::ptr> *in_write, std::set<tcp::ptr> *out_write,
    const std::set<tcp::ptr> *in_except, std::set<tcp::ptr> *out_except) {

    fd_set read, write, except;
    fd_set *readPtr = fill(in_read, read);
    fd_set *writePtr = fill(in_write, write);
    fd_set *exceptPtr = fill(in_except, except);
    int toRet = ::select(0, readPtr, writePtr, exceptPtr, nullptr);
    if (toRet > 0) {
        fill(in_read, read, out_read);
        fill(in_write, write, out_write);
        fill(in_except, except, out_except);
    }
    return toRet;
}

inline uint32_t to_fixed(float value, float min, float precision) {
    return (value - min) / precision;
}

inline float from_fixed(uint32_t value, float min, float precision) {
    return value * precision + min;
}

}   // namespace net
