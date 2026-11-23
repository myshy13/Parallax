#pragma once

#include "Raylib/collision.hpp"
#include "Renderer/renderer.hpp"
#include <cstdint>
#include <raylib.h>
#include <raymath.h>
#include <vector>

class World {
public:
  World(Renderer &renderer);
  ~World();

  // getters
  const Model &getGroundModel() const { return ground; }
  const Model &getShipModel() const { return ship; }
  float getGravity() const { return gravity; }
  float getDayPhase() const { return elapsed / dayLengthSecs; };

  // Everything the player can't walk through; built once, nothing here moves.
  const std::vector<OBB> &getColliders() const { return colliders; }

  void update(float dt);
  void draw(); // upload to renderer

private:
  uint32_t seed{};
  float elapsed{0.f};
  float dayLengthSecs{-1.f};
  static constexpr float GROUND_PLANE_SIZE = 120.f;
  // Thick so a fast fall can't skip through it in one step.
  static constexpr float GROUND_THICKNESS = 2.f;

  Renderer &renderer;
  Model ground;
  Model ship;
  Matrix shipTransform{MatrixIdentity()};
  std::vector<OBB> colliders;

  float gravity = 9.8f;
};
