#pragma once

#include "Raylib/collision.hpp"
#include "Renderer/renderer.hpp"
#include <cstdint>
#include <raylib.h>
#include <raymath.h>
#include <vector>

constexpr float DAY_RAMP_START =
    0.0f; // sun height where light starts; keep >= 0
constexpr float DAY_RAMP_END = 0.25f; // sun height of full daylight

constexpr float AMBIENT_DAY_CEIL = 0.3f;
constexpr float AMBIENT_NIGHT_FLOOR = 0.15f;

class World {
public:
  World(Renderer &renderer);
  ~World();

  // getters
  const Model &getGroundModel() const { return ground; }
  const Model &getShipModel() const { return ship; }
  float getGravity() const { return gravity; }
  float getDayPhase() const { return elapsed / dayLengthSecs; }
  float getDayLength() const { return dayLengthSecs; }

  // Everything the player can't walk through; built once, nothing here moves.
  const std::vector<OBB> &getColliders() const { return colliders; }

  void update(float dt);
  void draw(); // upload to renderer

  Vector3 getSunDirection() const;
  float getDayFactor() const;

private:
  uint32_t seed{};
  float elapsed{0.f};
  float dayLengthSecs{-1.f};
  static constexpr float GROUND_PLANE_SIZE = 120.f;
  // Thick so a fast fall can't skip through it in one step.
  static constexpr float GROUND_THICKNESS = 2.f;

  Renderer &renderer;
  Lighting &lighting;
  Model ground;
  Model ship;
  Matrix shipTransform{MatrixIdentity()};
  std::vector<OBB> colliders;

  float gravity = 9.8f;
};
