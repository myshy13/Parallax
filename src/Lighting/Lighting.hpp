#pragma once

#include "Lighting/light.hpp"
#include <raylib.h>
#include <vector>

constexpr int MAX_LIGHTS = 16; // must stay in sync with shaders
constexpr Vector3 defaultAmbient = {0.3, 0.3, 0.3};

struct LitShader {
  const Shader *shader;
  int dirLoc{-1};
  int colourLoc{-1};
  int countLoc{-1};
  int ambientLoc{-1};
};

class Lighting {
private:
  std::vector<Light> lights;
  Vector3 ambient{defaultAmbient};
  int nextId{0};

  std::vector<LitShader> litShaders;

  Light *findLight(int id, std::vector<Light> &lights);

public:
  int createLight(Vector3 toLight,
                  Color colour); // toLight points at the sun
  const Light *getLight(int id);
  void setLight(int id, const Light &newLight);
  void removeLight(int id);
  void addShader(const Shader &);

  void setAmbient(Color ambient);

  void upload();
};
