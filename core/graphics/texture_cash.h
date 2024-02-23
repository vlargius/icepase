#pragma once

#include <SDL_image.h>
#include <SDL_render.h>
#include <memory>
#include <static_constructor.h>
#include <string>
#include <unordered_map>

#include "texture.h"

namespace graphics {

namespace {
static Texture::ptr load_texture(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surface = IMG_Load(path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to load texture: %s", path);
        std::cerr << "failed to load texture: " << path << std::endl;
        return nullptr;
    }

    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return std::make_shared<Texture>(w, h, texture);
}
std::unordered_map<const char *, Texture::ptr> name_texture;
}   // namespace

namespace texture_cash {
inline extern Texture::ptr get(const char *name) {
    auto out = name_texture.find(name);
    if (out == name_texture.end()) {
        std::cerr << "can't load texture: " << name << std::endl;
        return nullptr;
    }
    return out->second;
}

inline extern void load(SDL_Renderer *renderer) {
    name_texture["penguin"] = load_texture(renderer, "./assets/penguin.png");
    name_texture["ball"] = load_texture(renderer, "./assets/ball.png");
}

}   // namespace texture_cash

}   // namespace graphics