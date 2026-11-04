#pragma once
#include <raylib.h>
#include <vector>

struct DrawCommand {
  const Mesh *mesh;
  const Material *material; // shader + uniforms/textures
  Matrix transform;
};

struct Batch {
  const Mesh *mesh;
  const Material *material;
  std::vector<Matrix> transforms;
};

class Renderer {
public:
  void submit(const DrawCommand &command) { queue.push_back(command); }
  void flush();

private:
  std::vector<DrawCommand> queue{};
};