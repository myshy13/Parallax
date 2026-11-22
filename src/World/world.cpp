#include "world.hpp"
#include "Lighting/light.hpp"
#include "Renderer/renderer.hpp"
#include "World/seedHelpers.hpp"
#include "rlgl.h"
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

World::World(Renderer &renderer)
    : renderer(renderer), lighting(renderer.getLighting()) {
  seed = rand();
  Mesh groundPlaneMesh =
      GenMeshPlane(GROUND_PLANE_SIZE, GROUND_PLANE_SIZE, 1, 1);
  ground = LoadModelFromMesh(groundPlaneMesh);
  ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
  ground.materials[0].shader = renderer.getGroundShader();

  ship = LoadModel("assets/models/ship.glb");
  for (int i = 0; i < ship.materialCount; i++) {
    ship.materials[i].shader = renderer.getShader();
  }

  shipTransform =
      shipTransform *
      MatrixRotateXYZ({5.f * DEG2RAD, 10.f * DEG2RAD, -10.f * DEG2RAD});
  shipTransform = shipTransform * MatrixTranslate(15.f, -0.5f, 0.f);
  shipTransform = shipTransform * MatrixScale(0.8f, 1.f, 0.8f);

  Shader shader = renderer.getGroundShader();
  int seedOffsetLoc = GetShaderLocation(shader, "seedOffset");

  Vector2 seedOffset = hashSeed(seed);
  SetShaderValue(shader, seedOffsetLoc, &seedOffset, SHADER_UNIFORM_VEC2);

  dayLengthSecs = hashDayLength(seed);
  elapsed =
      0.23f *
      dayLengthSecs; // player first starts in the world just before sunrise
}

void World::update(float dt) {
  elapsed += dt;
  elapsed = std::fmod(elapsed, dayLengthSecs);

  Vector3 newSunDirection = getSunDirection();
  int sunLight = renderer.getSunLight();
  lighting.setLight(sunLight,
                    Light{sunLight, newSunDirection,
                          ColorBrightness(sunColor, getDayFactor() - 1.0f)});
  unsigned char ambient =
      255 * std::lerp(AMBIENT_NIGHT_FLOOR, AMBIENT_DAY_CEIL, getDayFactor());
  lighting.setAmbient({ambient, ambient, ambient, 255});
}

void World::draw() {
  renderer.submit(
      DrawCommand{&ground.meshes[0], &ground.materials[0], MatrixIdentity()});
  for (int i = 0; i < ship.meshCount; i++) {
    renderer.submit(DrawCommand{
        &ship.meshes[i], &ship.materials[ship.meshMaterial[i]], shipTransform});
  }
}

float World::getDayFactor() const {
  float height = getSunDirection().y;
  float t = Clamp((height - DAY_RAMP_START) / (DAY_RAMP_END - DAY_RAMP_START),
                  0.0f, 1.0f);

  return t * t * (3.0f - 2.0f * t);
}

World::~World() {
  for (int i = 0; i < ship.materialCount; i++) {
    if (ship.materials[i].maps[MATERIAL_MAP_ALBEDO].texture.id ==
        rlGetTextureIdDefault())
      continue;
    UnloadTexture(ship.materials[i].maps[MATERIAL_MAP_ALBEDO].texture);
  }

  UnloadModel(ship);
  UnloadModel(ground);
}

constexpr float TAU = 2.f * PI;
constexpr float SUN_MODIFIER = .2f;

Vector3 World::getSunDirection() const {
  float phase = getDayPhase();
  float angle = (0.5 - phase) * TAU;

  Vector3 vector = {std::sinf(angle), std::cosf(angle), SUN_MODIFIER};
  vector = Vector3Normalize(vector);

  return vector;
}
