#pragma once

#include <SDL.h>
#include <memory>

#include "texture.h"

namespace graphics {
template <class RendererType>
class SpriteGen {
  public:
    using ptr = std::shared_ptr<SpriteGen>;

    Texture::ptr texture;

    SDL_Color color {255, 255, 255, 255};
    float scale = 1.f;
    float angle = 0.f;
    SDL_FPoint location{0.f, 0.f};

    SpriteGen() { RendererType::get().add(this); }
    ~SpriteGen() { RendererType::get().remove(this); }

    inline void debug(const SDL_Rect &view) const {
        SDL_Point size {40, 40};
        SDL_FRect dstRect;
        dstRect.w = size.x * scale;
        dstRect.h = size.y * scale;
        dstRect.x = location.x * view.w + view.x - dstRect.w / 2;
        dstRect.y = location.y * view.h + view.y - dstRect.h / 2;

        SDL_SetRenderDrawColor(RendererType::get().raw(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRectF(RendererType::get().raw(), &dstRect);
    }

    inline void render(const SDL_Rect &view) const {
        if (!texture) {
            debug(view);
            return;
        }

        // SDL_SetTextureColorMod(texture->raw(), color.r, color.g, color.b);

        SDL_Rect dstRect;
        dstRect.w = (int)(texture->width() * scale);
        dstRect.h = (int)(texture->height() * scale);
        dstRect.x = (int)(location.x * view.w + view.x - dstRect.w / 2);
        dstRect.y = (int)(location.y * view.h + view.y - dstRect.h / 2);

        SDL_RenderCopyEx(RendererType::get().raw(), texture->raw(), nullptr, &dstRect, angle, nullptr, SDL_FLIP_NONE);
    }
};
}   // namespace graphics