#pragma once

#include <iostream>
#include <string>

#include "engine.h"
#include "network.h"
#include "penguin.h"

using namespace server;

class Server final : public Engine {
  public:
    Server() {
        const std::string defaultAddress = "localhost:6666"; // todo move to default config
        const std::string addresStr = __argc > 1 ? __argv[1] : defaultAddress;
        const net::address::ptr address = net::make_address(addresStr);
        Network::get().init(*address);
        std::cerr << "server started at: " << addresStr << std::endl;
    }
  private:

    void setup() override {
    }

    void frame() override {
      Network::get().processInput();
      Network::get().processDisconnect();
      Network::get().respawn();
      Engine::frame();
      Network::get().processOutput();
    }

};