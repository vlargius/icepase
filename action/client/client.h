#pragma once

#include "engine.h"
#include "network.h"
#include "input.h"
#include "penguin.h"
#include "graphics/renderer.h"

using namespace client;

class Client : public Engine {
  public:
    bool init() {
        if (!graphics::renderer::get().init())
          return false;

        const std::string defaultAddress = "localhost:6666";   // todo move to default config
        const std::string defaultName = "test_client_0";       // todo move to default config
        const std::string addresStr = __argc > 1 ? __argv[1] : defaultAddress;
        const std::string name = __argc > 2 ? __argv[2] : defaultName;
        const net::address::ptr address = net::make_address(addresStr);
        Network::get().init(*address, name);

        std::cout << "client started with name - " << name << std::endl;
        return true;
    }

  private:
    void loop() override {
        SDL_Event event;

        while (isRunning) {
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_EventType::SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                } else if (event.type == SDL_EventType::SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSLASH) {
                    graphics::renderer::get().debug = !graphics::renderer::get().debug;
                } else {
                    processEvent(event);
                }
            }
            frame();
            timing::update();
        }
    }

    void processEvent(const SDL_Event& event) {
        switch (event.type) {
        case SDL_EventType::SDL_KEYDOWN: {
            Input::get().process(Input::Action::Press, event.key.keysym.sym);
            break;
        }
        case SDL_EventType::SDL_KEYUP: {
            Input::get().process(Input::Action::Release, event.key.keysym.sym);
            break;
        }
        }
    }

    void frame() override {
        Input::get().update();
        Engine::frame();
        Network::get().processInput();
        graphics::renderer::get().render();
        Network::get().processOutput();
    }
};