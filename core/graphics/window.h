#pragma once

#include <SDL.h>

namespace graphics {

class Window {
  public:
    ~Window() { SDL_DestroyWindow(rawPtr); }

    void create(int size_x, int size_y) {
        if (rawPtr)
            return;

        rawPtr = SDL_CreateWindow("", 50, 50, size_x, size_y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!rawPtr) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "window creation failed");
        }
    }

    SDL_Window *get() { return rawPtr; }
    void setTitle(const std::string &title) { SDL_SetWindowTitle(rawPtr, title.c_str()); }

  private:
    SDL_Window *rawPtr = nullptr;
};

}   // namespace graphics