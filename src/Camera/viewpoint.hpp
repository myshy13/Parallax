#pragma once

#include <raylib.h>
#include <raymath.h>

constexpr Vector3 FORWARD = {0.0f, 0.0f, -1.0f};

struct Viewpoint {
  Vector3 position{0, 0, 0};
  Quaternion rotation{0, 0, 0, 1};

  void applyTo(Camera3D &camera) const {
    camera.position = position;
    camera.up = {0, 1, 0};
    camera.target = Vector3Add(
        position, Vector3RotateByQuaternion(FORWARD, rotation));
  }
};