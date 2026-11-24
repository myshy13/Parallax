#pragma once

#include "Assets/assets.hpp"
#include "Camera/viewpoint.hpp"
#include "Input/input.hpp"
#include "Raylib/collision.hpp"
#include "Renderer/renderer.hpp"
#include "World/world.hpp"
#include <raylib.h>
#include <raymath.h>

// Player handles movement and provides a viewpoint
class Player {
private:
  // radians per look unit
  static constexpr float LOOK_SENSITIVITY = .002f;
  // 90 would break LookAt
  static constexpr float MAX_PITCH = 89.f * DEG2RAD;
  static constexpr float TOP_SPEED = 4.5f;
  static constexpr float TERMINAL_VELOCITY = 50.f;
  static constexpr float EYE_DROP = .3f;
  static constexpr float COLLISION_SKIN = .0005f; // metres

  static constexpr float JUMP_SPEED = 4.f;

  // Steeper than this is a wall, not a floor.
  static constexpr float MAX_WALKABLE_SLOPE = 45.f * DEG2RAD;
  // How far down to look for ground after a move, to stay on it going downhill.
  static constexpr float GROUND_SNAP = .1f;
  // Extra push past a surface so float noise doesn't re-trigger the contact.
  static constexpr float CONTACT_SLOP = .0001f;
  static constexpr int MAX_RESOLVE_PASSES = 4;

  Transform transform{{0, 0, 0}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.8f, 1.5f, 0.8f}};
  Vector3 vel{0.0f, 0.0f, 0.0f};

  float yaw = 0.0f;
  float pitch = 0.0f;

  bool grounded{false}; // calculate first frame

  AssetHandle playerTextureHandle; // from assets
  Model playerModel;

  Renderer &renderer; // reference to engine
  Input &input;

  const World &world;

  // transform.translation is the feet; the box and the cube centre sit above.
  Vector3 getBodyCentre() const;
  // Axis-aligned on purpose: it must not spin when the player looks around.
  OBB getBodyOBB() const;
  // Pushes the body out of everything it overlaps; true if it ended on a floor.
  bool resolveCollisions();

public:
  Player(Renderer &renderer, Assets &assets, Input &input, const World &world);
  ~Player();
  void update(float dt, float gravity);
  Viewpoint getViewpoint() const;
  void draw();

  const Vector3 &getVel() const { return vel; };
};
