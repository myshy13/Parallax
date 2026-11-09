#pragma once

#include "Renderer/renderer.hpp"
#include <raylib.h>
#include <raymath.h>
class World {
public:
  World(Renderer &renderer);
  ~World();

  // getters
  const Model &getGroundModel() const { return ground; }
  const Model &getShipModel() const { return ship; }
  float getGravity() const { return gravity; }
  
  void draw(); // upload to renderer

private:
  static constexpr float GROUND_PLANE_SIZE = 120.0f;

  Renderer &renderer;
  Model ground;
  Model ship;
  Matrix shipTransform{MatrixIdentity()};

  float gravity = 9.8f;
};
