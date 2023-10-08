#include <iostream>
#include <unordered_map>

#include "netutils.h"

void udp_test() {
    auto udp = net::make_udp();
    auto address = net::make_address("localhost:1111");
    udp->bind(*address);

    std::unordered_map<net::address, std::string> address_name;

    while (true) {
        char buffer[1024];
        net::address clientAddress;
        int length = udp->recieve_from(buffer, 1024, clientAddress);
        if (length > 1) {
            buffer[length] = '\0';
            if (address_name.find(clientAddress) == address_name.end()) {
                address_name[clientAddress] = buffer;
            }
            std::cout << address_name[clientAddress] << ": " << buffer << std::endl;
        }
    }
}

void tcp_test() {
    // auto receivingAddres = net::make_address("localhost:1111");
    auto receivingAddres = net::make_address("::1:6666", AF_INET6);
    auto listenSocket = net::make_tcp(AF_INET6);
    if (!listenSocket->bind(*receivingAddres)) {
        return;
    }

    listenSocket->listen();

    std::set<net::tcp::ptr> readBlockSockets;
    readBlockSockets.insert(listenSocket);

    std::set<net::tcp::ptr> readableSockets;
    std::set<net::tcp::ptr> writeableSockets;

    std::unordered_map<net::tcp::ptr, std::string> socket_name;

    bool isRunning = true;
    while (isRunning) {
        if (net::select(&readBlockSockets, &readableSockets, &writeableSockets, nullptr, nullptr, nullptr)) {
            for (const net::tcp::ptr &socket : readableSockets) {
                if (socket == listenSocket) {
                    net::address newClientAddress;
                    auto newSocket = listenSocket->accept(newClientAddress);
                    // newSocket->set_no_delay(true);
                    readBlockSockets.insert(newSocket);
                    writeableSockets.insert(newSocket);
                } else {
                    std::string msg;
                    const size_t MTU = 1200;
                    char buffer[MTU];
                    const int dataReceived = socket->recieve(buffer, MTU);
                    if (dataReceived > 0) {
                        buffer[dataReceived] = '\0';
                        if (socket_name.find(socket) == socket_name.end()) {
                            socket_name[socket] = buffer;
                            msg = "user '" + std::string(buffer) + "' regstered\n";
                        }
                        else
                            msg = "[" + socket_name[socket] + "]: " + std::string(buffer) + "\n";
                    } else {
                        msg = "user '" + std::string(buffer) + "' left\n";
                        readBlockSockets.erase(socket);
                        writeableSockets.erase(socket);
                    }
                    std::cout << msg;
                    for (auto listener : writeableSockets)
                        if(listener != socket)
                            listener->send(&msg[0], msg.size() + 1);
                }
            }
        }
    }
}

int main() {
    udp_test();
    // tcp_test();

    return 0;
}