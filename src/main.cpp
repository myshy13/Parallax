#include "Engine/engine.hpp"
#include <raylib.h>

int main() {
  Engine engine({1280, 720, "Parallax", 0, 1.0f / 60,
                 FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT});

  engine.run();
}
