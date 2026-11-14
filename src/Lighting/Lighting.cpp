#include "Lighting.hpp"
#include "Lighting/light.hpp"
#include <raylib.h>
#include <raymath.h>
#include <vector>

Light *Lighting::findLight(int id, std::vector<Light> &lights) {
  for (auto &l : lights)
    if (l.id == id)
      return &l;
  return nullptr;
}

void Lighting::addShader(const Shader &shader) {
  // ==== shader locations ==== //
  int dir = GetShaderLocation(shader, "lightDir[0]");
  int colour = GetShaderLocation(shader, "lightColour[0]");
  int lightCount = GetShaderLocation(shader, "lightCount");
  int ambient = GetShaderLocation(shader, "ambientColour");

  litShaders.emplace_back(&shader, dir, colour, lightCount, ambient);
}

void Lighting::setAmbient(Color newAmbient) {
  ambient.x = newAmbient.r / 255.0f;
  ambient.y = newAmbient.g / 255.0f;
  ambient.z = newAmbient.b / 255.0f;
  // alpha is ignored
}

// alpha is ignored
int Lighting::createLight(Vector3 toLight, Color colour) {
  if (lights.size() >= MAX_LIGHTS)
    return -1;
  int id = nextId++;
  Light light = {id, Vector3Normalize(toLight), colour};

  lights.push_back(light);

  return light.id;
}

const Light *Lighting::getLight(int id) { return findLight(id, lights); }

// id is ignored here
void Lighting::setLight(int id, const Light &newLight) {
  Light *light = findLight(id, lights);
  if (light == nullptr)
    return;
  *light = newLight;
  light->id = id;                            // ensure the id isn't broken
  light->dir = Vector3Normalize(light->dir); // shader expects a unit vector
}

void Lighting::removeLight(int id) {
  std::erase_if(lights, [&](const Light &l) { return id == l.id; });
}

void Lighting::upload() {
  Vector3 directions[MAX_LIGHTS] = {};
  Vector3 colours[MAX_LIGHTS] = {};
  int lightCount = lights.size();

  for (int i = 0; i < MAX_LIGHTS && i < (int)lights.size(); i++) {
    const Light &l = lights[i];
    colours[i] = l.colourToVector();
    directions[i] = l.dir;
  }

  for (const LitShader &litShader : litShaders) {
    SetShaderValue(*litShader.shader, litShader.countLoc, &lightCount,
                   SHADER_UNIFORM_INT);
    SetShaderValueV(*litShader.shader, litShader.colourLoc, colours,
                    SHADER_UNIFORM_VEC3, MAX_LIGHTS);
    SetShaderValueV(*litShader.shader, litShader.dirLoc, directions,
                    SHADER_UNIFORM_VEC3, MAX_LIGHTS);
    SetShaderValue(*litShader.shader, litShader.ambientLoc, &ambient,
                   SHADER_UNIFORM_VEC3);
  }
}
