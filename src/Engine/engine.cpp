#include "engine.hpp"
#include "Debug/debug.hpp"
#include "Input/inputSource.hpp"
#include "Player/player.hpp"
#include "Renderer/renderer.hpp"
#include <raylib.h>

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
    : window(conf), renderer(), player(renderer, assets, input), debug(player),
      input(makeInputSource()) {
  camera = {
      {10.0f, 10.0f, 10.0f}, {0, 0, 0}, {0, 1, 0}, 90.0f, CAMERA_PERSPECTIVE};
}

void Engine::run() {
  DisableCursor();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    // ==== update ==== //
    input.update();
    player.update(dt, camera);
    debug.update();

    // ==== upload ==== //
    player.draw();

    // ==== draw ==== //
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);

    DrawGrid(40, 1.0f);
    renderer.flush();

    EndMode3D();
    // ==== overlays ==== //
    debug.drawOverlays();
    EndDrawing();
  }
}