#pragma once

#include "Input/input.hpp"
#include "Player/player.hpp"
#include "World/world.hpp"
#include <deque>
#include <raylib.h>

constexpr int AVERAGE_FRAMETIME_COUNT = 30;

class Debug {
private:
  const Player &player;
  float speed{0.0f};

  bool enabled{false};

  std::deque<float> lastFrametimes = {}; // last dts

  const Input &input;
  const World &world;

public:
  Debug(const Player &player, const Input &input, const World &world);
  void drawOverlays(float dt);
  void update();
};