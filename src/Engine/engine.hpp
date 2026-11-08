#pragma once

#include "Assets/assets.hpp"
#include "Debug/debug.hpp"
#include "Input/input.hpp"
#include "Player/player.hpp"
#include "Renderer/renderer.hpp"
#include <raylib.h>

struct EngineConfig {
  int width{1280};
  int height{720};

  const char *title{"Parallax"};

  int targetFps = 0; // 0 means automatic (raylib handles it)
  double fixedDt = 1.0 / 60.0;

  unsigned flags = FLAG_VSYNC_HINT;
};

struct Window {
  explicit Window(const EngineConfig &conf);
  ~Window() { CloseWindow(); }

  // copy protection
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;
};

class Engine {
public:
  explicit Engine(const EngineConfig &conf);
  ~Engine();

  // copy protection
  Engine(const Engine &) = delete;
  Engine &operator=(const Engine &) = delete;

  void run();

private:
  static constexpr float GROUND_PLANE_SIZE = 120.0f;
  float gravity = 9.8f; // m/s²

  Window window; // must stay first
  Assets assets;
  Renderer renderer;

  Debug debug;
  Player player;
  Model groundPlane;

  Input input;
  Camera3D camera;
};