#pragma once

#include "Lighting/light.hpp"
#include <optional>
#include <raylib.h>
#include <vector>

constexpr int MAX_LIGHTS = 4; // must stay in sync with shaders
constexpr Vector3 defaultAmbient = {0.3, 0.3, 0.3};

// a copy of the shader (only its id is used) and where its uniforms live
struct LitShader {
  Shader shader;
  int dirLoc{-1};
  int colourLoc{-1};
  int countLoc{-1};
  int ambientLoc{-1};
  int viewLoc{-1};
};

class Lighting {
private:
  std::vector<Light> lights;
  Vector3 ambient{defaultAmbient};
  int nextId{0};

  std::vector<LitShader> litShaders;

  Light *findLight(int id);

public:
  // toLight points from the surface towards the light; returns -1 if rejected
  int createLight(Vector3 toLight, Color colour);
  // a copy, because the lights vector can move when lights are added or removed
  std::optional<Light> getLight(int id) const;
  void setLight(int id, const Light &newLight);
  void removeLight(int id);
  void addShader(const Shader &shader);

  void setAmbient(Color newAmbient);

  void upload(Vector3 viewPos);
};
