#include "world.hpp"
#include "Lighting/light.hpp"
#include "Renderer/renderer.hpp"
#include "World/seedHelpers.hpp"
#include "rlgl.h"
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

// ==== ship collider bounds, in the model's own space ==== //
constexpr BoundingBox SHIP_HULL_BOUNDS{{-6.02f, 0.0f, -2.78f},
                                       {6.25f, 3.53f, 2.79f}};
// Thin planks, so each wing is its own box and the space under it stays open.
constexpr BoundingBox SHIP_WING_NEG_Z_BOUNDS{{-0.03f, 2.17f, -8.33f},
                                             {1.97f, 2.37f, -2.33f}};
constexpr BoundingBox SHIP_WING_POS_Z_BOUNDS{{-0.03f, 2.117f, 2.30f},
                                             {2.00f, 2.37f, 8.33f}};

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

  // Scale, then rotate, then move, so the scale never skews or shifts the ship.
  shipTransform =
      MatrixScale(0.8f, 1.f, 0.8f) *
      MatrixRotateXYZ({5.f * DEG2RAD, 10.f * DEG2RAD, -10.f * DEG2RAD}) *
      MatrixTranslate(12.f, -0.5f, 0.f);

  // ==== colliders ==== //
  // The ground's top face is y = 0, so the box hangs below it.
  Transform groundBox{{0.f, -GROUND_THICKNESS / 2, 0.f},
                      QuaternionIdentity(),
                      {GROUND_PLANE_SIZE, GROUND_THICKNESS, GROUND_PLANE_SIZE}};
  colliders.push_back(GetOBBFromTransform(groundBox, Vector3One()));
  colliders.push_back(GetOBBFromMatrix(shipTransform, SHIP_HULL_BOUNDS));
  colliders.push_back(GetOBBFromMatrix(shipTransform, SHIP_WING_NEG_Z_BOUNDS));
  colliders.push_back(GetOBBFromMatrix(shipTransform, SHIP_WING_POS_Z_BOUNDS));

  Shader shader = renderer.getGroundShader();
  int seedOffsetLoc = GetShaderLocation(shader, "seedOffset");

  Vector2 seedOffset = hashSeed(seed);
  SetShaderValue(shader, seedOffsetLoc, &seedOffset, SHADER_UNIFORM_VEC2);

  dayLengthSecs = hashDayLength(seed);
  elapsed =
      0.248f *
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
  static Mesh sphereMesh = GenMeshSphere(3.0f, 16, 16);
  static Model sphere = LoadModelFromMesh(sphereMesh);
  renderer.submit(
      DrawCommand{&sphere.meshes[0], &sphere.materials[0], MatrixIdentity()});
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
