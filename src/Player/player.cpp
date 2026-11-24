#include "player.hpp"
#include "Assets/assets.hpp"
#include "Input/input.hpp"
#include "Input/inputState.hpp"
#include "Raylib/collision.hpp"
#include "Renderer/renderer.hpp"
#include "World/world.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>

Player::Player(Renderer &renderer, Assets &assets, Input &input,
               const World &world)
    : playerTextureHandle(assets.loadAsset("assets/textures/player.png")),
      renderer(renderer), input(input), world(world) {
  Mesh playerMesh = GenMeshCube(
      1.0f, 1.0f, 1.0f); // 1.0f because scale gets applied through transform
  playerModel = LoadModelFromMesh(playerMesh);

  SetMaterialTexture(&playerModel.materials[0], MATERIAL_MAP_ALBEDO,
                     assets.getTexture(playerTextureHandle));
  playerModel.materials[0].shader = renderer.getShader();
}

// Frees the mesh and maps only; the shader and texture belong to Renderer and
// Assets.
Player::~Player() { UnloadModel(playerModel); }

Vector3 Player::getBodyCentre() const {
  return Vector3Add(transform.translation, {0.0f, transform.scale.y / 2, 0.0f});
}

OBB Player::getBodyOBB() const {
  Transform body{getBodyCentre(), QuaternionIdentity(), transform.scale};
  OBB obb = GetOBBFromTransform(body, Vector3One());

  // Shrunk a hair so resting on a surface isn't counted as overlapping it.
  obb.halfExtents = Vector3SubtractValue(obb.halfExtents, COLLISION_SKIN);
  return obb;
}

// ==== collision response ==== //
bool Player::resolveCollisions() {
  const float minFloorNormalY = std::cos(MAX_WALKABLE_SLOPE);
  bool onFloor                = false;

  if (transform.translation.y < -0.1f) {
    transform.translation.y = 0.f;
  }

  for (int pass = 0; pass < MAX_RESOLVE_PASSES; pass++) {
    bool anyHit = false;

    for (const OBB &collider : world.getColliders()) {
      Contact contact = GetContactOBB(getBodyOBB(), collider);
      if (!contact.hit)
        continue;
      anyHit = true;

      Vector3 normal = contact.normal; // points out of the collider
      float push     = contact.depth + CONTACT_SLOP;

      if (std::fabs(normal.y) >= minFloorNormalY) {
        // Floor or ceiling: push straight up or down, so a slope can't be
        // crept down.
        transform.translation.y +=
            std::copysign(push / std::fabs(normal.y), normal.y);
        if (normal.y > 0.0f) {
          onFloor = true;
          vel.y   = std::max(vel.y, 0.0f);
        } else {
          vel.y = std::min(vel.y, 0.0f);
        }
      } else {
        // Wall: push sideways only, so a leaning wall can't be climbed. The
        // velocity into the wall is dropped and the rest slides along it.
        Vector3 sideways = {normal.x, 0.0f, normal.z};
        float length     = Vector3Length(sideways);
        Vector3 side     = Vector3Scale(sideways, 1.0f / length);

        transform.translation = Vector3Add(transform.translation,
                                           Vector3Scale(side, push / length));
        float into = Vector3DotProduct(vel, side);
        if (into < 0.0f)
          vel = Vector3Subtract(vel, Vector3Scale(side, into));
      }
    }

    if (!anyHit)
      break;
  }
  return onFloor;
}

void Player::draw() {
  Matrix scale =
      MatrixScale(transform.scale.x, transform.scale.y, transform.scale.z);
  Matrix rotation = QuaternionToMatrix(transform.rotation);
  Vector3 centre = getBodyCentre();
  Matrix translation = MatrixTranslate(centre.x, centre.y, centre.z);
  Matrix transformMatrix = scale * rotation * translation;
  renderer.submit(DrawCommand{&playerModel.meshes[0], &playerModel.materials[0],
                              transformMatrix});
}

Viewpoint Player::getViewpoint() const {
  // Pitch applies first, then yaw; the other order makes the camera roll.
  Quaternion viewRotation = QuaternionMultiply(
      transform.rotation, QuaternionFromAxisAngle({1.0f, 0.0f, 0.0f}, pitch));

  Vector3 head = Vector3Add(transform.translation,
                            {0.0f, transform.scale.y - EYE_DROP, 0.0f});

  return Viewpoint{head, viewRotation};
}

void Player::update(float dt, float gravity) {
  // No dt here: a mouse delta is already the movement for this frame.
  Vector2 look = input.getLook();
  yaw -= look.x * LOOK_SENSITIVITY;
  pitch = Clamp(pitch - look.y * LOOK_SENSITIVITY, -MAX_PITCH, MAX_PITCH);

  // The body only yaws, so the cube doesn't tilt when looking up.
  transform.rotation = QuaternionFromAxisAngle({0.0f, 1.0f, 0.0f}, yaw);

  Vector3 lookForward = Vector3RotateByQuaternion(FORWARD, transform.rotation);

  Vector3 moveForward = lookForward;
  moveForward.y = 0.0f;
  moveForward = Vector3Normalize(moveForward);

  Vector3 right =
      Vector3RotateByQuaternion({1.0f, 0.0f, 0.0f}, transform.rotation);
  Vector3 moveRight = right;

  moveRight.y = 0.0f;
  moveRight = Vector3Normalize(moveRight);

  Vector2 move = input.getMove();

  Vector3 moveDir = {0, 0, 0};
  moveDir = Vector3Add(moveDir, Vector3Scale(moveForward, move.y));
  moveDir = Vector3Add(moveDir, Vector3Scale(moveRight, move.x));

  if (Vector3Length(moveDir) > 1.0f) {
    moveDir = Vector3Normalize(moveDir);
  }

  if (grounded && input.down(Action::Jump)) {
    vel.y += JUMP_SPEED;
    grounded = false;
  }

  // Push and damp in one exact step, so top speed doesn't depend on frame rate.
  const float dampBase = 0.7f; // per 1/60 s
  const float damping = powf(dampBase, dt * 60.0f);
  vel.x = vel.x * damping + moveDir.x * TOP_SPEED * (1.0f - damping);
  vel.z = vel.z * damping + moveDir.z * TOP_SPEED * (1.0f - damping);

  // ==== move, then push out ==== //
  bool wasGrounded = grounded; // after the jump above, so a jump isn't snapped

  // y uses this frame's starting vel.y, so vel.y is updated after the move.
  Vector3 step = {vel.x * dt, vel.y * dt - 0.5f * gravity * dt * dt,
                  vel.z * dt};
  transform.translation = Vector3Add(transform.translation, step);
  vel.y -= gravity * dt;
  vel.y = std::max(vel.y, -TERMINAL_VELOCITY);
  grounded = resolveCollisions();

  // Walking downhill leaves the ground a little below; look for it and drop on.
  if (wasGrounded && !grounded && vel.y <= 0.0f) {
    Vector3 savedPos = transform.translation;
    Vector3 savedVel = vel;
    transform.translation.y -= GROUND_SNAP;
    grounded = resolveCollisions();
    if (!grounded) {
      transform.translation = savedPos;
      vel = savedVel;
    }
  }
}
