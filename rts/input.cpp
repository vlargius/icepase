#include "input.h"

#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <graphics/renderer.h>

#include "network.h"
#include "move_command.h"

void Input::process(Action action, int key_code) {
    switch (key_code) {
    case SDLK_RETURN: {
        Network::get().tryStartGame();
        break;
    }

    default:
        break;
    }
}

void Input::click(int32_t x, int32_t y, uint8_t button) {
    switch (button)
    {
    case SDL_BUTTON_LEFT:
    {
        addCommand(graphics::Renderer::get().relativeX(x), graphics::Renderer::get().relativeY(y));
        break;
    }
    case SDL_BUTTON_RIGHT:
    {
        break;
    }
    default:
        break;
    }
}

void Input::update() {}

void Input::clear() { commandList.clear(); }

Input::Input() {}

void Input::addCommand(float x, float y) {
    Command::ptr command;

    Penguin::ptr controlledObject = std::static_pointer_cast<Penguin>(World::get().find([](const Object::ptr object) {
        return object->getType() == Penguin::type &&
               std::static_pointer_cast<Penguin>(object)->userId == Network::get().getUserId();
    }));

    if (controlledObject)
        command = MoveCommand::create(controlledObject->netId, x, y);

    if (command)
        commandList.add(command);
}
