#include "player.hpp"
#include "Assets/assets.hpp"
#include "Input/input.hpp"
#include "Renderer/renderer.hpp"
#include <raylib.h>
#include <raymath.h>

Player::Player(Renderer &renderer, Assets &assets, Input &input)
    : playerTextureHandle(assets.loadAsset("assets/textures/player.png")),
      renderer(renderer), assets(assets), input(input) {
  Mesh playerMesh = GenMeshCube(0.8f, 2.0f, 0.8f); // cubic player for now
  playerModel = LoadModelFromMesh(playerMesh);

  SetMaterialTexture(&playerModel.materials[0], MATERIAL_MAP_ALBEDO,
                     assets.getTexture(playerTextureHandle));
}

void Player::draw() {
  renderer.submit(DrawCommand{&playerModel.meshes[0], &playerModel.materials[0],
                              playerModel.transform});
}

#include "raylib.h"
#include "raymath.h"

Vector3 GetForwardVector(Vector3 rotationRadians) {
  // Define Raylib's standard forward vector (often negative Z or positive Z
  // depending on your convention)
  Vector3 defaultForward = {0.0f, 0.0f, 1.0f};

  // Create a rotation matrix from the Euler angles (Pitch, Yaw, Roll)
  Matrix rotationMatrix = MatrixRotateXYZ(rotationRadians);

  // Transform the default forward vector by the rotation matrix
  Vector3 forward = Vector3Transform(defaultForward, rotationMatrix);

  // Normalize it to ensure it is a unit vector of length 1
  return Vector3Normalize(forward);
}

void Player::update(float dt, Camera &camera) {
  pos += Vector3Scale(vel, dt);

  camera.position = pos;
  camera.target = pos + GetForwardVector(look);
}