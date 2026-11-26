#pragma once

#include <cfloat>
#include <cmath>
#include <raylib.h>
#include <raymath.h>

struct OBB {
  Vector3 centre;
  Vector3 axes[3];     // Local X, Y, Z axes (normalised)
  Vector3 halfExtents; // Scaled half-size along each local axis
};

static const int OBB_EDGES[12][2] = {
    {0, 1}, {1, 3}, {3, 2}, {2, 0}, // Z- face
    {4, 5}, {5, 7}, {7, 6}, {6, 4}, // Z+ face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting struts
};

// Few sides is enough for a debug edge; a square beam still reads as a line.
constexpr int OBB_EDGE_SIDES = 4;

// Nearest point to p on the segment from a to b.
inline Vector3 ClosestPointOnSegment(Vector3 a, Vector3 b, Vector3 p) {
  Vector3 ab = Vector3Subtract(b, a);
  float t =
      Vector3DotProduct(Vector3Subtract(p, a), ab) / Vector3DotProduct(ab, ab);
  return Vector3Add(a, Vector3Scale(ab, Clamp(t, 0.0f, 1.0f)));
}

// Edges are cylinders (wide lines aren't portable), sized by distance from
// viewPos so they keep about the same thickness on screen.
inline void DrawOBB(const OBB &obb, Color colour, Vector3 viewPos,
                    float radiusPerMetre) {
  Vector3 extents[3] = {Vector3Scale(obb.axes[0], obb.halfExtents.x),
                        Vector3Scale(obb.axes[1], obb.halfExtents.y),
                        Vector3Scale(obb.axes[2], obb.halfExtents.z)};

  Vector3 corners[8];
  for (int i = 0; i < 8; i++) {
    Vector3 corner = obb.centre;
    corner =
        Vector3Add(corner, (i & 1) ? extents[0] : Vector3Negate(extents[0]));
    corner =
        Vector3Add(corner, (i & 2) ? extents[1] : Vector3Negate(extents[1]));
    corner =
        Vector3Add(corner, (i & 4) ? extents[2] : Vector3Negate(extents[2]));
    corners[i] = corner;
  }

  for (int e = 0; e < 12; e++) {
    Vector3 start = corners[OBB_EDGES[e][0]];
    Vector3 end = corners[OBB_EDGES[e][1]];
    Vector3 nearest = ClosestPointOnSegment(start, end, viewPos);
    float radius = Vector3Distance(nearest, viewPos) * radiusPerMetre;
    DrawCylinderEx(start, end, radius, radius, OBB_EDGE_SIDES, colour);
  }
}

// ==== building boxes ==== //
// Convert a Raylib Transform + base size into an OBB
inline OBB GetOBBFromTransform(Transform t, Vector3 baseSize) {
  OBB obb;
  obb.centre = t.translation;

  // Rotate the standard world axes by the transform's quaternion
  obb.axes[0] = Vector3RotateByQuaternion(Vector3{1, 0, 0}, t.rotation);
  obb.axes[1] = Vector3RotateByQuaternion(Vector3{0, 1, 0}, t.rotation);
  obb.axes[2] = Vector3RotateByQuaternion(Vector3{0, 0, 1}, t.rotation);

  // Apply scale to half-extents
  obb.halfExtents = Vector3{(baseSize.x * 0.5f) * std::fabs(t.scale.x),
                            (baseSize.y * 0.5f) * std::fabs(t.scale.y),
                            (baseSize.z * 0.5f) * std::fabs(t.scale.z)};
  return obb;
}

// Box for a model's local bounds drawn with m; exact unless m shears.
inline OBB GetOBBFromMatrix(const Matrix &m, const BoundingBox &bounds) {
  Vector3 size = Vector3Subtract(bounds.max, bounds.min);
  Vector3 localCentre = Vector3Scale(Vector3Add(bounds.min, bounds.max), 0.5f);
  Vector3 origin = Vector3Transform(Vector3Zero(), m);

  // Where each local axis ends up; its length is that axis' scale.
  Vector3 image[3] = {Vector3Subtract(Vector3Transform({1, 0, 0}, m), origin),
                      Vector3Subtract(Vector3Transform({0, 1, 0}, m), origin),
                      Vector3Subtract(Vector3Transform({0, 0, 1}, m), origin)};

  OBB obb;
  obb.centre = Vector3Transform(localCentre, m);
  obb.axes[0] = Vector3Normalize(image[0]);
  obb.axes[1] = Vector3Normalize(image[1]);
  obb.axes[2] = Vector3Normalize(image[2]);
  obb.halfExtents = Vector3{size.x * 0.5f * Vector3Length(image[0]),
                            size.y * 0.5f * Vector3Length(image[1]),
                            size.z * 0.5f * Vector3Length(image[2])};
  return obb;
}

// ==== contact ==== //
struct Contact {
  bool hit{false};
  Vector3 normal{0, 0, 0}; // unit; the way to push a so it leaves b
  float depth{0.0f};       // how far to push a along normal
};

// Separating Axis Theorem over 15 axes; on a hit, keeps the axis of least
// overlap, which is the shortest way to push a out of b.
inline Contact GetContactOBB(const OBB &a, const OBB &b) {
  constexpr float MIN_AXIS_LENGTH = 1e-4f; // parallel edges cross to ~zero
  constexpr float EDGE_AXIS_BIAS = 1e-4f;  // on a near tie, prefer a face axis

  const float ha[3] = {a.halfExtents.x, a.halfExtents.y, a.halfExtents.z};
  const float hb[3] = {b.halfExtents.x, b.halfExtents.y, b.halfExtents.z};
  const Vector3 gap = Vector3Subtract(a.centre, b.centre);

  Contact best;
  best.depth = FLT_MAX;

  // Returns false when this axis separates the boxes.
  auto overlapsOn = [&](Vector3 axis, float bias) {
    float length = Vector3Length(axis);
    if (length < MIN_AXIS_LENGTH)
      return true; // degenerate axis; the face axes already cover it
    axis = Vector3Scale(axis, 1.0f / length);

    // How far each box reaches along this axis from its centre.
    float reachA = 0.0f;
    float reachB = 0.0f;
    for (int i = 0; i < 3; i++) {
      reachA += ha[i] * std::fabs(Vector3DotProduct(a.axes[i], axis));
      reachB += hb[i] * std::fabs(Vector3DotProduct(b.axes[i], axis));
    }

    float distance = Vector3DotProduct(gap, axis); // b's centre to a's
    float overlap = reachA + reachB - std::fabs(distance);
    if (overlap <= 0.0f)
      return false;

    if (overlap + bias < best.depth) {
      best.depth = overlap;
      best.normal = distance < 0.0f ? Vector3Negate(axis) : axis;
    }
    return true;
  };

  // Face axes of a, then of b.
  for (int i = 0; i < 3; i++) {
    if (!overlapsOn(a.axes[i], 0.0f))
      return Contact{};
  }
  for (int i = 0; i < 3; i++) {
    if (!overlapsOn(b.axes[i], 0.0f))
      return Contact{};
  }
  // Edge axes: every edge of a against every edge of b.
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Vector3 edgeAxis = Vector3CrossProduct(a.axes[i], b.axes[j]);
      if (!overlapsOn(edgeAxis, EDGE_AXIS_BIAS))
        return Contact{};
    }
  }

  best.hit = true;
  return best;
}
