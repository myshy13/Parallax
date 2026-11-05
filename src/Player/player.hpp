#pragma once

#include "Assets/assets.hpp"
#include "Input/input.hpp"
#include "Renderer/renderer.hpp"
#include <raylib.h>
#include <raymath.h>

// Player handles movement and camera
class Player {
private:
  // radians per look unit
  static constexpr float LOOK_SENSITIVITY = 0.002f;
  // 90 would break LookAt
  static constexpr float MAX_PITCH = 89.0f * DEG2RAD;
  static constexpr float TOP_SPEED = 5.f;
  static constexpr float EYE_DROP = 0.3f;

  Transform transform{{0, 0, 0}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.8f, 1.5f, 0.8f}};
  Vector3 vel{0.0f, 0.0f, 0.0f};

  float yaw = 0.0f;
  float pitch = 0.0f;

  AssetHandle playerTextureHandle; // from assets
  Model playerModel;

  Renderer &renderer; // reference to engine
  Input &input;

public:
  Player(Renderer &renderer, Assets &assets, Input &input);
  void update(float dt, Camera &camera);
  void draw();

  const Vector3 &getVel() const { return vel; };
};
