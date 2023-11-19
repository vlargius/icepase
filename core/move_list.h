#pragma once

#include <deque>

#include "move.h"

class MoveList {
  public:
    bool empty() const { return moves.empty(); }
    size_t size() const { return moves.size(); }
    std::deque<Move>::iterator begin() { return moves.begin(); }
    std::deque<Move>::iterator end() { return moves.end(); }

    const Move& getLastMove() const { return moves.back(); }
    const float getLastTimestamp() const { return lastTimestamp; }

    Move &operator[](int i) { return moves[i]; }
    void clear() { moves.clear(); }

    const Move& add(const InputState& state, float timestamp) {
      const float timeDelta = lastTimestamp >= 0.f ? timestamp - lastTimestamp : 0.f;
      moves.emplace_back(state, timestamp, timeDelta);
      lastTimestamp = timestamp;
      return moves.back();
    }

    void add(const Move &move) {
        const float timeStamp = move.getTimestamp();
        const float deltaTime = lastTimestamp >= 0.f ? timeStamp - lastTimestamp : 0.f;
        lastTimestamp = timeStamp;
        moves.emplace_back(move.getState(), timeStamp, deltaTime);
    }

    void remove(float timestamp) {
      while (!moves.empty() && moves.front().getTimestamp() <= timestamp) {
        moves.pop_front();
      }
    }

  private:
    std::deque<Move> moves;
    float lastTimestamp = -1.f;
};