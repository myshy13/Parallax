#include "world.hpp"
#include "Renderer/renderer.hpp"
#include "World/seedHelpers.hpp"
#include "rlgl.h"
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

World::World(Renderer &renderer) : renderer(renderer) {
  // note: This will stay the same each build without an srand. ok for now
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
  elapsed = 0.2f * dayLengthSecs;
}

void World::update(float dt) {
  elapsed += dt;
  elapsed = fmod(elapsed, dayLengthSecs);
}

void World::draw() {
  renderer.submit(
      DrawCommand{&ground.meshes[0], &ground.materials[0], MatrixIdentity()});
  for (int i = 0; i < ship.meshCount; i++) {
    renderer.submit(DrawCommand{
        &ship.meshes[i], &ship.materials[ship.meshMaterial[i]], shipTransform});
  }
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
