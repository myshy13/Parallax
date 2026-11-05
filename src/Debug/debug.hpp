#pragma once

#include "Player/player.hpp"
#include <raylib.h>

class Debug {
private:
  const Player &player;
  float speed{0.0f};

  bool enabled{false};

public:
  Debug(const Player &player);
  void drawOverlays();
  void update();
};