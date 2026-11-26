#pragma once

#include <raylib.h>
#include <raymath.h>

constexpr float SUN_RADIUS = .05f;
constexpr float DOME_RADIUS = 500.f;

constexpr Vector2 CLOUD_WIND{.03f, .01f};

class Sky {
public:
  Sky();
  ~Sky();

  // no copy operations
  Sky(const Sky &) = delete;
  Sky &operator=(const Sky &) = delete;

  void update(float dt, Color skyColour, Color horizonColour,
              Vector3 sunDirection, Color sunColour);
  void draw(Vector3 viewpointPos);

  void setCloudOrigin(Vector2 origin);

private:
  Shader skyShader;

  Model dome;

  Vector2 cloudOffset{};

  // ==== locations ==== //
  int skyColourLoc{-1};
  int horizonColourLoc{-1};
  int sunDirLoc{-1};
  int sunColourLoc{-1};
  int viewPosLoc{-1};
  int cloudOffsetLoc{-1};
  int sunRadiusLoc{-1};
};