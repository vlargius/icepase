#pragma once

#include <engine.h>
#include <graphics/renderer.h>

#include "input.h"
#include "network.h"

class Client : public Engine {
  public:
    bool init() {
        if (!graphics::Renderer::get().init())
            return false;

        const std::string defaultAddress = "localhost:6666";   // todo move to default config
        const std::string defaultName = "client_0";       // todo move to default config
        const std::string name = __argc > 1 ? __argv[1] : defaultName;
        const bool isServer = __argc <= 2;
        const std::string addressStr = isServer ? defaultAddress : __argv[2];
        const net::address::ptr address = net::make_address(addressStr);

        graphics::Renderer::get().driver.window.setTitle(name);

        timing::frameTimer.duration = .016f;

        if (isServer) {
            return Network::get().initMaster(*address, name);
        } else {
            return Network::get().initPeer(*address, name);
        }
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
                    graphics::Renderer::get().debug = !graphics::Renderer::get().debug;
                } else {
                    processEvent(event);
                }
            } else {
                timing::update();
                frame();
            }
        }
    }

    void processEvent(const SDL_Event &event) {
        switch (event.type) {
        case SDL_EventType::SDL_KEYDOWN: {
            Input::get().process(Input::Action::Press, event.key.keysym.sym);
            break;
        }
        case SDL_EventType::SDL_KEYUP: {
            Input::get().process(Input::Action::Release, event.key.keysym.sym);
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            Input::get().click(event.button.x, event.button.y, event.button.button);
            break;
        }
        }
    }

    void frame() override {
        Input::get().update();

        if (Network::get().isDelayed()) {
            Network::get().processInput();
        } else {
            World::get().update();
            Network::get().processInput();
            Network::get().processOutput();
        }

        graphics::Renderer::get().render();
    }
};