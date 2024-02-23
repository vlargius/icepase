#pragma once

#include <SDL.h>
#include <memory>

#include "texture.h"

namespace graphics {

class Sprite;

extern void register_sprite(const Sprite *);
class Sprite {
  public:
    using ptr = std::shared_ptr<Sprite>;

    Texture::ptr texture;

    SDL_Color color {255, 255, 255, 255};
    float scale = 1.f;
    float angle = 0.f;
    SDL_FPoint location{0.f, 0.f};

    Sprite() { register_sprite(this); }
    ~Sprite() { register_sprite(this); }

    inline void debug(SDL_Renderer *renderer, const SDL_Rect &view) const {
        SDL_Point size {40, 40};
        SDL_FRect dstRect;
        dstRect.w = size.x * scale;
        dstRect.h = size.y * scale;
        dstRect.x = location.x * view.w + view.x - dstRect.w / 2;
        dstRect.y = location.y * view.h + view.y - dstRect.h / 2;

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRectF(renderer, &dstRect);
    }

    inline void render(SDL_Renderer *renderer, const SDL_Rect &view) const {
        if (!texture) {
            debug(renderer, view);
            return;
        }

        SDL_SetTextureColorMod(texture->raw(), color.r, color.g, color.b);

        SDL_Rect dstRect;
        dstRect.w = (int)(texture->width() * scale);
        dstRect.h = (int)(texture->height() * scale);
        dstRect.x = (int)(location.x * view.w + view.x - dstRect.w / 2);
        dstRect.y = (int)(location.y * view.h + view.y - dstRect.h / 2);

        SDL_RenderCopyEx(renderer, texture->raw(), nullptr, &dstRect, angle, nullptr, SDL_FLIP_NONE);
    }
};
}   // namespace graphics