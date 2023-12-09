#pragma once

#include <SDL.h>


namespace graphics {

class Window {
  public:
    Window() {
        rawPtr = SDL_CreateWindow("icepace", 50, 50, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!rawPtr) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "window creation failed");
        }
    }

    SDL_Window *get() { return rawPtr; }

  private:
    SDL_Window* rawPtr;
};

}   // namespace graphics