#pragma once

#include <engine.h>
// #include <graphics/renderer.h>

#include "network.h"

class Client : public Engine {
  public:

  bool init() {
    // if (!graphics::renderer::get().init())
    //     return false;

    const std::string defaultAddress = "localhost:6666";   // todo move to default config
    const std::string defaultName = "test_client_0";       // todo move to default config
    const std::string addressStr = __argc > 1 ? __argv[1] : defaultAddress;
    const std::string name = __argc > 2 ? __argv[2] : defaultName;

    if (addressStr.find(':') == std::string::npos) {
        return Network::get().initPeer(stoi(addressStr), name);
    } else {
        const net::address::ptr address = net::make_address(addressStr);
        return Network::get().initMaster(*address, name);
    }
    return true;
  }

  private:
};