#pragma once

#include <iostream>

#include "window.h"

namespace graphics {
class Driver {
  public:
    const SDL_Color backgroundColor{190, 151, 254, 0};
    Window window;

    Driver() {}
    ~Driver() {
        SDL_Quit();
        SDL_DestroyRenderer(rawRenderer);
    }

    bool init(int size_x = 800, int size_y = 600) {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
            return false;
        }

        window.create(size_x, size_y);
        if (!window.get())
            return false;

        rawRenderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED);
        if (!rawRenderer) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "driver creation failed");
            return false;
        }

        SDL_SetRenderDrawColor(rawRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, SDL_ALPHA_OPAQUE);
        SDL_RenderSetLogicalSize(rawRenderer, size_x, size_y);
        return true;
    }

    void clear() {
        SDL_SetRenderDrawColor(rawRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(rawRenderer);
    }
    void present() { SDL_RenderPresent(rawRenderer); }
    SDL_Renderer *get() { return rawRenderer; }

  private:

    SDL_Renderer *rawRenderer;
    SDL_Rect viewPort;
};
}   // namespace graphics