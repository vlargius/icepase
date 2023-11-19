#pragma once

#include <vector>

#include "driver.h"
#include "sprite.h"
#include "texture_cash.h"

namespace graphics {

class renderer {
  public:
    using Sprite = SpriteGen<renderer>;

    bool debug = false;

    static renderer &get() {
        static renderer instance;
        return instance;
    }

    void add(const Sprite *sprite) {
      sprites.push_back(sprite);
    }

    void remove(const Sprite *sprite) {
      sprites.erase(std::find(sprites.begin(), sprites.end(), sprite));
    }

    bool init() {
        if (!driver.init())
          return false;

        SDL_Rect viewport;
        SDL_RenderGetLogicalSize(driver.get(), &viewport.w, &viewport.h);
        view.x = viewport.w / 2;
        view.y = viewport.h / 2;
        view.w = 100;
        view.h = 100;

        texture_cash::load(raw());

        return true;
    }

    SDL_Renderer * raw() { return driver.get(); }

    void render() {
        driver.clear();
        renderSprites();
        driver.present();
    }

  private:
    Driver driver;
    SDL_Rect view;
    std::vector<const Sprite *> sprites;

    void renderSprites() {
        for (const Sprite *sprite : sprites) {
          if (debug)
            sprite->debug(view);
          else
            sprite->render(view);
        }
    }
};

}   // namespace graphics