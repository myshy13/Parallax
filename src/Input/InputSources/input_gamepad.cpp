#include <raylib.h>
#include <raymath.h>

#include <cstddef>
#include <memory>

#include "Input/inputSource.hpp"
#include "Input/inputState.hpp"

class InputGamepad : public InputSource {
private:
  // Per second, so the feel doesn't change with the frame rate.
  static constexpr float CURSOR_SPEED  = 600.0f;  // pixels
  static constexpr float LOOK_SPEED    = 1200.0f; // mouse-delta units
  static constexpr float DRIFT_MAXIMUM = 0.1f;

  // SDL_GameControllerDB (zlib), copied next to the other assets
  static constexpr const char *MAPPINGS_FILE = "assets/gamecontrollerdb.txt";

  struct Bind {
    Action action;
    GamepadButton button;
  };

  // Xbox names; an action can sit on several buttons, and a button on several actions
  static constexpr Bind BINDS[] = {
      {Action::Jump, GAMEPAD_BUTTON_RIGHT_FACE_DOWN},     // A
      {Action::Click, GAMEPAD_BUTTON_RIGHT_FACE_DOWN},    // A: menus only
      {Action::OpenDebug, GAMEPAD_BUTTON_LEFT_FACE_DOWN}, // Left DPad down
  };

  Vector2 cursor{0, 0};
  bool cursorVisible{true};
  bool windowResized{true};

  MouseCursor pendingCursor = MOUSE_CURSOR_DEFAULT;

public:
  InputGamepad() {
    // GLFW's built-in table lacks newer firmware GUIDs, so such a pad reads as
    // all zeros without this
    if (char *db = LoadFileText(MAPPINGS_FILE)) {
      SetGamepadMappings(db);
      UnloadFileText(db);
    }
  }

  void poll(InputState &state) override {
    if (!IsGamepadAvailable(0))
      return;

    if (IsWindowResized())
      windowResized = true;
    if (windowResized) {
      cursor = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
      windowResized = false;
    }
    state = {};

    for (const Bind &b : BINDS) {
      // Click is menu-only: A also jumps, and must not click while playing
      if (b.action == Action::Click && !cursorVisible)
        continue;
      if (IsGamepadButtonDown(0, b.button)) {
        state.actions.set(static_cast<size_t>(b.action));
      }
    }

    Vector2 leftMovement = {GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X),
                            GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y)};
    Vector2 rightMovement = {GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X),
                             GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)};

    if (leftMovement.x < DRIFT_MAXIMUM && leftMovement.x > -DRIFT_MAXIMUM)
      leftMovement.x = 0.0f;
    if (leftMovement.y < DRIFT_MAXIMUM && leftMovement.y > -DRIFT_MAXIMUM)
      leftMovement.y = 0.0f;
    if (rightMovement.x < DRIFT_MAXIMUM && rightMovement.x > -DRIFT_MAXIMUM)
      rightMovement.x = 0.0f;
    if (rightMovement.y < DRIFT_MAXIMUM && rightMovement.y > -DRIFT_MAXIMUM)
      rightMovement.y = 0.0f;

    // The cursor only moves in menus; while playing the stick walks instead
    float dt = GetFrameTime();
    if (cursorVisible) {
      cursor += leftMovement * (CURSOR_SPEED * dt);
      cursor = Vector2Clamp(
          cursor, {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    }

    state.pointer = cursor;
    state.look = rightMovement * (LOOK_SPEED * dt);
    state.move = {leftMovement.x, -leftMovement.y};
  };

  void setCursorStyle(MouseCursor style) override { pendingCursor = style; }

  void drawCursor() override {
    if (cursorVisible) {
      DrawRectangle(cursor.x - 5, cursor.y - 5, 10, 10, WHITE);
    }
    pendingCursor = MOUSE_CURSOR_DEFAULT;
  }

  void setMouseLook(bool mouseLook) override {
    cursorVisible = !mouseLook;
    cursor = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
  }
};

std::unique_ptr<InputSource> makeInputSource() {
  return std::make_unique<InputGamepad>();
}