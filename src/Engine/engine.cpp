#include "engine.hpp"
#include "Debug/debug.hpp"
#include "Input/inputSource.hpp"
#include "Input/inputState.hpp"
#include "Player/player.hpp"
#include "Renderer/renderer.hpp"
#include <raylib.h>
#include <raymath.h>

Window::Window(const EngineConfig &conf) {
  SetConfigFlags(conf.flags);
  InitWindow(conf.width, conf.height, conf.title);
  if (conf.targetFps > 0)
    SetTargetFPS(conf.targetFps);

#ifndef __EMSCRIPTEN__
  ChangeDirectory(GetApplicationDirectory());
#endif

  // window sizing
  int monitor = GetCurrentMonitor();
  int width = GetMonitorWidth(monitor);
  int height = GetMonitorHeight(monitor);
  SetWindowSize(width, height);

  SetWindowPosition(0, 0);

  SetExitKey(KEY_NULL);
}

Engine::Engine(const EngineConfig &conf)
    : window(conf), renderer(), debug(player, input),
      player(renderer, assets, input), input(makeInputSource()) {
  camera = {
      {10.0f, 10.0f, 10.0f}, {0, 0, 0}, {0, 1, 0}, 100.0f, CAMERA_PERSPECTIVE};

  Mesh groundPlaneMesh =
      GenMeshPlane(GROUND_PLANE_SIZE, GROUND_PLANE_SIZE, 1, 1);
  groundPlane = LoadModelFromMesh(groundPlaneMesh);
  groundPlane.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
  groundPlane.materials[0].shader = renderer.getGroundShader();
}

Engine::~Engine() { UnloadModel(groundPlane); }

void Engine::run() {
  DisableCursor();

  while (!WindowShouldClose()) {
    if (input.pressed(Action::Click) && !IsCursorHidden())
      DisableCursor();
    float dt = GetFrameTime();
    // ==== update ==== //
    input.update();
    player.update(dt, gravity);
    player.getViewpoint().applyTo(camera);
    debug.update();

    // ==== upload ==== //
    player.draw();

    // ==== draw ==== //
    renderer.submit(DrawCommand{&groundPlane.meshes[0],
                                &groundPlane.materials[0], MatrixIdentity()});
    renderer.flush(camera);

    BeginDrawing();
    ClearBackground(BLACK);

    const Texture2D &renderTexture = renderer.getRenderTexture();
    DrawTexturePro(renderTexture,
                   {0, 0, static_cast<float>(renderTexture.width),
                    -static_cast<float>(renderTexture.height)},
                   {0, 0, static_cast<float>(GetScreenWidth()),
                    static_cast<float>(GetScreenHeight())},
                   {0, 0}, 0.0f, WHITE);

    // ==== overlays ==== //
    debug.drawOverlays(dt);
    EndDrawing();
  }
}
