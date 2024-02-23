#pragma once

#include <vector>

#include "driver.h"
#include "sprite.h"
#include "texture_cash.h"

namespace graphics {

class Renderer {
  public:

    bool debug = false;
    Driver driver;

    static Renderer &get() {
        static Renderer instance;
        return instance;
    }

    float relativeX(int x) const { return (x - view.x) / (float)view.w; }
    float relativeY(int y) const { return (y - view.y) / (float)view.h; }

    void add(const Sprite *sprite) {
      sprites.push_back(sprite);
    }

    void remove(const Sprite *sprite) {
      sprites.erase(std::find(sprites.begin(), sprites.end(), sprite));
    }

    bool init() {
        int sizeX = 800;
        int sizeY = 600;
        if (!driver.init(sizeX, sizeY))
          return false;

        SDL_Rect viewport;
        SDL_RenderGetLogicalSize(driver.get(), &viewport.w, &viewport.h);
        view.x = viewport.w / 2;
        view.y = viewport.h / 2;
        view.w = 100;
        view.h = 100;

        graphics::texture_cash::load(raw());
        return true;
    }

    SDL_Renderer * raw() { return driver.get(); }

    void render() {
      driver.clear();
      renderSprites();
      driver.present();
    }

  private:
    SDL_Rect view;
    std::vector<const Sprite *> sprites;

    void renderSprites() {
        for (const Sprite *sprite : sprites) {
          if (debug)
            sprite->debug(raw(), view);
          else
            sprite->render(raw(), view);
        }
    }
};

inline extern void register_sprite(const Sprite *sprite) { graphics::Renderer::get().add(sprite); }

}   // namespace graphics