#include "penguin.h"

#include "network.h"

namespace server {

void Penguin::update() {
  if (Proxy::ptr client = Network::get().getClient(userId)) {
    for (const Move& move : client->unprocessedMoves) {
      const float delta = move.getTimeDelta();
      process(move.getState(), delta);
      simulate(delta);
    }
    client->unprocessedMoves.clear();

    Network::get().updateFields(netId, Fields::Position);
  }
}

Penguin::ptr Penguin::create() {
  Penguin::ptr pengiun = std::make_shared<Penguin>();
  pengiun->netId = Network::get().link(pengiun);
  return pengiun;
}

void Penguin::process(const InputState &input, float time_delta){
  position.x += input.getLateral() * time_delta;
  position.y -= input.getLongitudinal() * time_delta;
}

void Penguin::simulate(const float time_delta) {

}
}   // namespace server