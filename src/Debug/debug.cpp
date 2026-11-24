#include "debug.hpp"
#include "Input/inputState.hpp"
#include "Raylib/collision.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <iomanip>
#include <numeric>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <sstream>

Debug::Debug(const Player &player, const Input &input, const World &world)
    : player(player), input(input), world(world) {}

void Debug::update() {
  speed = Vector3Length(player.getVel());
  if (input.pressed(Action::OpenDebug))
    enabled = !enabled;
};

float getAverage(const std::deque<float> &ar) {
  auto sum = std::accumulate(ar.begin(), ar.end(), 0.0f);
  return sum / ar.size();
}

void Debug::drawOverlays(float dt) {
  lastFrametimes.push_front(dt);

  while (lastFrametimes.size() > AVERAGE_FRAMETIME_COUNT)
    lastFrametimes.pop_back();
  if (!enabled)
    return;

  int fps = GetFPS();

  int ROW = 0;
  Vector2 startPos = {10, 10};

  int ROW_HEIGHT = 30;
  int FONTSIZE = 20;

  float normalTime = std::clamp(world.getDayPhase(), 0.0f, 1.0f);
  int totalSeconds = static_cast<int>(std::round(normalTime * SECS_PER_DAY));
  if (totalSeconds >= SECS_PER_DAY) {
    totalSeconds = 0;
  }
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;
  std::stringstream worldTime{};

  worldTime << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2)
            << minutes << ":" << std::setw(2) << seconds;

  auto drawText = [&](const char *text, Color c = WHITE) {
    DrawText(text, startPos.x, startPos.y + ROW * ROW_HEIGHT, FONTSIZE, c);
    ROW++;
  };

  drawText(TextFormat("Speed: %.2f", speed));
  drawText(TextFormat("FPS: %i", fps));
  drawText(TextFormat("Frame time (MS): %.2f", dt * 1000));
  drawText(TextFormat("Average frame time (MS): %.2f",
                      getAverage(lastFrametimes) * 1000));
  drawText(TextFormat("World Day Phase: %.3f", world.getDayPhase()));
  drawText(TextFormat("World Day length: %.1f", world.getDayLength()));
  drawText(TextFormat("Current Time (24hrs): %s", worldTime.str().c_str()));
}

void Debug::drawBoxes() {
  if (!enabled)
    return;
   // visual overlays
  constexpr Color LINES_COLOUR = WHITE;
  auto &colliders = world.getColliders();

  rlCheckRenderBatchLimit(colliders.size() * 24);
  
  rlBegin(RL_LINES);
  rlColor4ub(LINES_COLOUR.r, LINES_COLOUR.g, LINES_COLOUR.b, LINES_COLOUR.a);

  for (const OBB &o : colliders) {
    DrawOBB(o);
  }
  rlEnd();
}
