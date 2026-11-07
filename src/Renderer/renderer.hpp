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
  Renderer(); // initialises the shader
  ~Renderer();

  void submit(const DrawCommand &command) { queue.push_back(command); }
  void flush();

  const Shader &getShader() const { return shader; }
  const Shader &getGroundShader() const { return groundShader; }

private:
  std::vector<DrawCommand> queue{};

  Shader shader;       // todo: move to dedicated lighting class
  Shader groundShader; // makes gridlines

  Shader loadInstancedShader(const char *name);
};