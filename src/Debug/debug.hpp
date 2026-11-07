#pragma once

#include "Player/player.hpp"
#include <deque>
#include <raylib.h>

constexpr int AVERAGE_FRAMETIME_COUNT = 30;

class Debug {
private:
  const Player &player;
  float speed{0.0f};

  bool enabled{false};

  std::deque<float> lastFrametimes = {}; // last dts

public:
  Debug(const Player &player);
  void drawOverlays(float dt);
  void update();
};