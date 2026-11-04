#pragma once

#include <raylib.h>

#include <functional>
#include <memory>
#include <optional>

#include "Input/inputSource.hpp"
#include "Input/inputState.hpp"

class Input {
private:
  std::unique_ptr<InputSource> source;

  InputState current;
  InputState previous;

  bool keyboardOpenLast{false}; // was the keyboard open last frame

public:
  Input(std::unique_ptr<InputSource> source);

  void update();
  void draw();
  void drawCursor();

  // ==== getters ==== //
  bool down(Action) const;
  bool pressed(Action) const;
  Vector2 getMove() const;
  Vector2 getLook() const;
  Vector2 getPointer() const;
  float getScroll() const;
  std::optional<int> getHotbarSlot() const;

  // ==== setters ==== //
  void setMouseLook(bool mouseLook);
  void setCursorStyle(MouseCursor style);

  bool openSystemKeyboard(const std::string &title, const std::string &initial,
                          bool masked, std::function<void(std::string)> onDone);
};
