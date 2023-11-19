#pragma once

#include <string>
#include <iostream>

#ifdef _WIN
#include <winsock2.h>
#include <WS2tcpip.h>

namespace net {
using socket_type = SOCKET;
using socklen_t = int;

inline int get_last_error() { return WSAGetLastError(); }

inline void report(const std::string &msg = "", int error = 0) {
    char buffer[256];
    buffer [0] = '\0';
    if (!error)
        WSAGetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
        buffer,
        sizeof(buffer),
        nullptr);

    if (msg.empty())
        std::cerr << "error::net: - " << (error && buffer[0] ? std::string(buffer) : std::to_string(error)) << std::endl;
    else
        std::cerr << "error::net: " << msg  << " - " << (error && buffer[0] ? std::string(buffer) : std::to_string(error)) << std::endl;
}

inline void closesocket(socket_type) {}

inline bool set_blocking(socket_type socket_, bool is_blocking) {
    u_long state = !is_blocking;
    return ::ioctlsocket(socket_, FIONBIO, &state) == 0;
}
}
#else

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

namespace net {

using socket_type = int;
const socket_type INVALID_SOCKET = -1;

inline int get_last_error() { assert("todo implement get last error" && false); return -1; }

inline void report(const std::string &msg = "", int error = 0) {
    std::cerr << "error::net:" + msg + " '" + std::string(strerror(error ? error : errno)) + "' " << std::endl; 
}

inline void closesocket(socket_type socket_) { ::close(socket_); }
inline bool set_blocking(socket_type socket_, bool is_blocking) {
    int flags = ::fcntl(socket_, F_GETFL);
    return ::fcntl(socket_, F_SETFL, flags | (is_blocking ? ~O_NONBLOCK : O_NONBLOCK));
}

}   // namespace net

#endif