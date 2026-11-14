#pragma once

#include <raylib.h>
struct Light {
  int id;
  Vector3 dir; // unit vector pointing TO the light (surface -> sun)
  // Vector3 pos;
  Color colour;

  // converts the colour to a shader-usable rgb colour (alpha is ignored)
  Vector3 colourToVector() const {
    return {colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f};
  };
};