#include "engine.hpp"
#include "Debug/debug.hpp"
#include "Input/inputSource.hpp"
#include "Input/inputState.hpp"
#include "Player/player.hpp"
#include "Renderer/renderer.hpp"
#include <algorithm>
#include <raylib.h>
#include <raymath.h>

// Longest step the simulation takes; a longer frame runs slow, not in one jump.
constexpr float MAX_STEP_SECONDS = 0.05f;

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
    : window(conf), renderer(), world(renderer), debug(player, input, world),
      player(renderer, assets, input, world), input(makeInputSource()) {
  camera = {
      {10.0f, 10.0f, 10.0f}, {0, 0, 0}, {0, 1, 0}, 100.0f, CAMERA_PERSPECTIVE};
}

void Engine::run() {
  DisableCursor();

  while (!WindowShouldClose()) {
    if (input.pressed(Action::Click) && !IsCursorHidden())
      DisableCursor();
    float frameTime = GetFrameTime();
    float dt        = std::min(frameTime, MAX_STEP_SECONDS);
    // ==== update ==== //
    input.update();
    player.update(dt, world.getGravity());
    world.update(dt);
    player.getViewpoint().applyTo(camera);
    debug.update();

    // ==== upload ==== //
    player.draw();
    world.draw();

    // ==== draw ==== //
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
    debug.drawOverlays(frameTime);
    EndDrawing();
  }
}
