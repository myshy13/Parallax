#pragma once

#include <raylib.h>

constexpr float COLOUR_CHANNEL_MAX = 255.0f;

// converts a colour to a shader-usable rgb vector (alpha is ignored)
inline Vector3 colourToVector(Color colour) {
  return {colour.r / COLOUR_CHANNEL_MAX, colour.g / COLOUR_CHANNEL_MAX,
          colour.b / COLOUR_CHANNEL_MAX};
}

struct Light {
  int id;
  Vector3 dir; // unit vector pointing TO the light (surface -> sun)
  Color colour;
};
