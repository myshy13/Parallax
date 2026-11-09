#include "world.hpp"
#include "Renderer/renderer.hpp"
#include "rlgl.h"
#include <raylib.h>
#include <raymath.h>

World::World(Renderer &renderer) : renderer(renderer) {
  Mesh groundPlaneMesh =
      GenMeshPlane(GROUND_PLANE_SIZE, GROUND_PLANE_SIZE, 1, 1);
  ground = LoadModelFromMesh(groundPlaneMesh);
  ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
  ground.materials[0].shader = renderer.getGroundShader();

  ship = LoadModel("assets/models/ship.glb");
  for (int i = 0; i < ship.materialCount; i++) {
    ship.materials[i].shader = renderer.getShader();
  }

  shipTransform = shipTransform * MatrixRotateZ(-5 * DEG2RAD);
  shipTransform = shipTransform * MatrixTranslate(15, -0.5, 0);
  shipTransform = shipTransform * MatrixScale(0.8,0.8,0.8);
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
