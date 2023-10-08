#include <iostream>
#include <istream>
#include <string>
#include <future>

#include "netutils.h"

void udp_test() {
    auto serverAddress = net::make_address("localhost:1111");
    auto udp = net::make_udp();
    udp->connect(*serverAddress);

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        udp->send(&msg[0], msg.size() + 1);
    }
}

void tcp_test() {
    auto serverAddress = net::make_address("::1:6666", AF_INET6);
    auto tcp = net::make_tcp(AF_INET6);
    if (!tcp->connect(*serverAddress)) {
        return;
    }

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);

        if (msg == "exit") {
            std::cout << "goodbye :)" << std::endl;
            return;
        }
        if (!msg.empty())
        {
            tcp->send(&msg[0], msg.size() + 1);
            msg.clear();
        }
        std::thread input([&]() {
            while (true) {
                const size_t MTU = 1200;
                char buffer[MTU];
                const int dataReceived = tcp->recieve(buffer, MTU);
                if (dataReceived > 0) {
                    buffer[dataReceived] = '\0';
                    std::cout << buffer;
                }
            }
        });
        input.detach();
    }
}

int main() {
    udp_test();
    // tcp_test();

    return 0;
}