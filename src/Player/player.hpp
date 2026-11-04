#pragma once

#include "Assets/assets.hpp"
#include "Input/input.hpp"
#include "Renderer/renderer.hpp"
#include <raylib.h>
#include <raymath.h>

class Player {
private:
  // initialises to {0,0,0}
  Vector3 pos{0.0f,2.0f, 0.0f}; // top of player
  Vector3 vel{0.0f, 0.0f, 0.0f};

  Vector3 look{}; // player looking direction. radians

  AssetHandle playerTextureHandle; // from assets
  Model playerModel;

  Renderer& renderer; // reference to engine
  Assets& assets; // refrence to engine's copy
  Input& input;

public:
  Player(Renderer &renderer, Assets &assets, Input& input);
  void update(float dt, Camera& camera);
  void draw();
};
