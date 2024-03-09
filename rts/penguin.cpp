#include "penguin.h"

#include <static_constructor.h>

#include "network.h"


Penguin::Penguin() {
    sprite = std::make_shared<graphics::Sprite>();
    sprite->texture = graphics::texture_cash::get("penguin");
    sprite->scale = 0.1f;
}

ObjFields Penguin::write(outstream &stream, const ObjFields fields) {
    ObjFields writtenFields;

    if (fields & Fields::Position) {
        stream.write(true);
        stream.write(position);
        writtenFields |= Fields::Position;
    } else
        stream.write(true);

    return writtenFields;
}

void Penguin::update() {
    const float dt = timing::frameTimer.duration;
    const float speed = .1f;

    position.x += (target.x - position.x) * dt;
    position.y += (target.y - position.y) * dt;

    sprite->location.x = position.x;
    sprite->location.y = position.y;
}

void Penguin::read(instream &stream) {
    bool hasState;
    stream.read(hasState);
    if (hasState) {
        stream.read(position);
    }
}

void Penguin::static_init() {
    static_constructor<&Penguin::static_init>::c.run();
    Penguin::Factory::add<Object, Penguin>();
}

Penguin::ptr Penguin::create() {
    Penguin::ptr pengiun = std::make_shared<Penguin>();
    pengiun->netId = Network::get().linker.get(pengiun, true);
    return pengiun;
}
