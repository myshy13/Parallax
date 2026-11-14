#include "Lighting.hpp"
#include "Lighting/light.hpp"
#include <algorithm>
#include <raylib.h>
#include <raymath.h>

// a direction shorter than this can't be normalised
constexpr float MIN_DIR_LENGTH = 1e-6f;

// ==== shaders ==== //
void Lighting::addShader(const Shader &shader) {
  int dirLoc     = GetShaderLocation(shader, "lightDir[0]");
  int colourLoc  = GetShaderLocation(shader, "lightColour[0]");
  int countLoc   = GetShaderLocation(shader, "lightCount");
  int ambientLoc = GetShaderLocation(shader, "ambientColour");
  int viewLoc    = GetShaderLocation(shader, "viewPos");

  litShaders.emplace_back(shader, dirLoc, colourLoc, countLoc, ambientLoc,
                          viewLoc);
}

// ==== ambient ==== //
// alpha is ignored
void Lighting::setAmbient(Color newAmbient) {
  ambient = colourToVector(newAmbient);
}

// ==== lights ==== //
Light *Lighting::findLight(int id) {
  for (auto &l : lights)
    if (l.id == id)
      return &l;
  return nullptr;
}

// alpha is ignored
int Lighting::createLight(Vector3 toLight, Color colour) {
  if (lights.size() >= MAX_LIGHTS)
    return -1;
  if (Vector3Length(toLight) < MIN_DIR_LENGTH) {
    TraceLog(LOG_WARNING, "Lighting: light direction has zero length");
    return -1;
  }
  int id      = nextId++;
  Light light = {id, Vector3Normalize(toLight), colour};

  lights.push_back(light);

  return light.id;
}

std::optional<Light> Lighting::getLight(int id) const {
  auto it = std::find_if(lights.begin(), lights.end(),
                         [&](const Light &l) { return l.id == id; });
  if (it == lights.end())
    return std::nullopt;
  return *it;
}

// newLight.id is ignored; the light keeps the id it was found by
void Lighting::setLight(int id, const Light &newLight) {
  Light *light = findLight(id);
  if (light == nullptr)
    return;
  if (Vector3Length(newLight.dir) < MIN_DIR_LENGTH) {
    TraceLog(LOG_WARNING, "Lighting: light direction has zero length");
    return;
  }
  *light     = newLight;
  light->id  = id;                           // ensure the id isn't broken
  light->dir = Vector3Normalize(light->dir); // shader expects a unit vector
}

void Lighting::removeLight(int id) {
  std::erase_if(lights, [&](const Light &l) { return id == l.id; });
}

// ==== upload ==== //
void Lighting::upload(Vector3 viewPos) {
  Vector3 directions[MAX_LIGHTS] = {};
  Vector3 colours[MAX_LIGHTS]    = {};
  int lightCount                 = static_cast<int>(lights.size());

  // createLight caps the count at MAX_LIGHTS, so this stays inside the arrays
  for (int i = 0; i < lightCount; i++) {
    colours[i]    = colourToVector(lights[i].colour);
    directions[i] = lights[i].dir;
  }

  for (const LitShader &litShader : litShaders) {
    SetShaderValue(litShader.shader, litShader.countLoc, &lightCount,
                   SHADER_UNIFORM_INT);
    SetShaderValueV(litShader.shader, litShader.colourLoc, colours,
                    SHADER_UNIFORM_VEC3, MAX_LIGHTS);
    SetShaderValueV(litShader.shader, litShader.dirLoc, directions,
                    SHADER_UNIFORM_VEC3, MAX_LIGHTS);
    SetShaderValue(litShader.shader, litShader.ambientLoc, &ambient,
                   SHADER_UNIFORM_VEC3);
    SetShaderValue(litShader.shader, litShader.viewLoc, &viewPos,
                   SHADER_UNIFORM_VEC3);
  }
}
