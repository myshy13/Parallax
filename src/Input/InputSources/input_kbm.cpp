#include <raylib.h>

#include <cstddef>
#include <iterator>
#include <memory>

#include "Input/inputSource.hpp"
#include "Input/inputState.hpp"

class InputKbm : public InputSource {
  static constexpr float SCROLL_SENSITIVITY = 1.5f;

  // ==== binds ==== //
  struct Bind {
    bool mouse; // true: a mouse button, false: a key
    int code;
  };

  // One row per Action, in enum order (the static_assert catches a missing
  // row, not a swapped one).
  static constexpr Bind BINDS[] = {
      {false, KEY_SPACE},        // Jump
      {true, MOUSE_BUTTON_LEFT}, // Click
      {false, KEY_K},            // OpenDebug
  };

  static_assert(std::size(BINDS) == static_cast<size_t>(Action::Count),
                "one bind per Action");

  MouseCursor pendingCursor = MOUSE_CURSOR_DEFAULT;
  bool styleChanged;

  void setCursorStyle(MouseCursor style) override {
    styleChanged = true;
    pendingCursor = style;
  };

  void drawCursor() override {
    if (styleChanged) {
      SetMouseCursor(pendingCursor);
    }
    pendingCursor = MOUSE_CURSOR_DEFAULT;
  };

  void poll(InputState &inputState) override {
    inputState = {};
    for (size_t i = 0; i < std::size(BINDS); i++) {
      const Bind &bind = BINDS[i];
      bool down =
          bind.mouse ? IsMouseButtonDown(bind.code) : IsKeyDown(bind.code);
      inputState.actions.set(i, down);
    }

    inputState.look = GetMouseDelta();
    inputState.pointer = GetMousePosition();
    inputState.scroll = GetMouseWheelMove() * SCROLL_SENSITIVITY;

    int key = GetKeyPressed();
    // 1-9 range
    if (key >= KEY_ONE && key < KEY_ONE + 9) {
      inputState.newHotBarSlot = key - KEY_ONE;
    }

    inputState.move = {0, 0};
    // Movement
    if (IsKeyDown(KEY_W)) {
      inputState.move.y += 1.0f;
    }

    if (IsKeyDown(KEY_A)) {
      inputState.move.x -= 1.0f;
    }

    if (IsKeyDown(KEY_S)) {
      inputState.move.y -= 1.0f;
    }

    if (IsKeyDown(KEY_D)) {
      inputState.move.x += 1.0f;
    }
  }

  void setMouseLook(bool mouseLook) override {
    if (mouseLook) {
      DisableCursor();
    } else {
      EnableCursor();
    }
  }
};

std::unique_ptr<InputSource> makeInputSource() {
  return std::make_unique<InputKbm>();
};
