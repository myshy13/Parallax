#include "debug.hpp"
#include <raylib.h>
#include <raymath.h>

Debug::Debug(const Player &player) : player(player) {}

void Debug::update() {
  speed = Vector3Length(player.getVel());
  if (IsKeyPressed(KEY_K))
    enabled = !enabled;
};

void Debug::drawOverlays() {
  if (!enabled) return;

  int fps = GetFPS();
  
  int ROW = 0;
  Vector2 startPos = {10, 10};

  int ROW_HEIGHT = 30;
  int FONTSIZE = 20;

  auto drawText = [&](const char* text, Color c = WHITE) {
    DrawText(text, startPos.x, startPos.y + ROW * ROW_HEIGHT, FONTSIZE, c);
    ROW++;
  };

  drawText(TextFormat("Speed: %.2f", speed));
  drawText(TextFormat("FPS: %i", fps));
}