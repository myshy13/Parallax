#include "player.hpp"
#include "Assets/assets.hpp"
#include "Input/input.hpp"
#include "Input/inputState.hpp"
#include "Renderer/renderer.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>

Player::Player(Renderer &renderer, Assets &assets, Input &input)
    : playerTextureHandle(assets.loadAsset("assets/textures/player.png")),
      renderer(renderer), input(input) {
  Mesh playerMesh = GenMeshCube(
      1.0f, 1.0f, 1.0f); // 1.0f because scale gets applied through transform
  playerModel = LoadModelFromMesh(playerMesh);

  SetMaterialTexture(&playerModel.materials[0], MATERIAL_MAP_ALBEDO,
                     assets.getTexture(playerTextureHandle));
  playerModel.materials[0].shader = renderer.getShader();
}

void Player::draw() {
  Matrix scale =
      MatrixScale(transform.scale.x, transform.scale.y, transform.scale.z);
  Matrix rotation = QuaternionToMatrix(transform.rotation);
  Matrix translation = MatrixTranslate(
      transform.translation.x, transform.translation.y + transform.scale.y / 2,
      transform.translation.z);
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

  auto blocked = [&]() { return transform.translation.y < 0; };

  transform.translation.x += vel.x * dt;
  if (blocked()) {
    transform.translation.x -= vel.x * dt; // how do i snap these values?
    vel.x = 0.0f;
  }
  transform.translation.y += vel.y * dt - 0.5f * gravity * pow(dt, 2);
  // After the move: the line above needs this frame's starting vel.y.
  vel.y -= gravity * dt;
  vel.y = std::max(vel.y, -TERMINAL_VELOCITY);
  if (blocked()) {
    transform.translation.y = 0.0f;
    vel.y = 0.0f;
    grounded = true;
  } else {
    grounded = false;
  }
  transform.translation.z += vel.z * dt;
  if (blocked()) {
    transform.translation.z -= vel.z * dt; // how do i snap these values?
    vel.z = 0.0f;
  }
}
