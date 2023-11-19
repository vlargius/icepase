#pragma once

#include <SDL.h>
#include <memory>

namespace graphics {
class Texture {
  public:
    using ptr = std::shared_ptr<Texture>;

    Texture(size_t width_, size_t height_, SDL_Texture *raw)
        : textureWidth(width_), textureHeight(height_), rawTexture(raw) {}

    size_t width() const { return textureWidth; }
    size_t height() const { return textureHeight; }
    SDL_Texture *raw() const { return rawTexture; }

  private:
    size_t textureWidth;
    size_t textureHeight;
    SDL_Texture *rawTexture;
};
}   // namespace graphics