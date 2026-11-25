#pragma once

#include <raylib.h>
#include <raymath.h>

constexpr float SUN_RADIUS = .09f;
constexpr float DOME_RADIUS = 500.f;

class Sky {
public:
  Sky();
  ~Sky();

  // no copy operations
  Sky(const Sky &) = delete;
  Sky &operator=(const Sky &) = delete;

  void update(Color skyColour, Vector3 sunDirection, Color sunColour);
  void draw(Vector3 viewpointPos);

private:
  Shader skyShader;

  Model dome;

  // ==== locations ==== //
  int skyColourLoc{-1};
  int sunDirLoc{-1};
  int sunColourLoc{-1};
  int sunRadiusLoc{-1};
};