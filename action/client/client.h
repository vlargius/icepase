#pragma once

#include "engine.h"
#include "network.h"
#include "input.h"
#include "penguin.h"

using namespace client;

class Client : public Engine {
  public:
    Client() {
        const std::string defaultAddress = "localhost:6666";   // todo move to default config
        const std::string defaultName = "test_client_0";   // todo move to default config
        const std::string addresStr = __argc > 1 ? __argv[1] : defaultAddress;
        const std::string name = __argc > 2 ? __argv[2] : defaultName;
        const net::address::ptr address = net::make_address(addresStr);
        Network::get().init(*address, name);
        std::cerr << "client started with name - " << name << std::endl;
    }

  private:

    void frame() override {
        Input::get().update();
        Engine::frame();
        Network::get().processInput();
        Network::get().processOutput();
    }
};