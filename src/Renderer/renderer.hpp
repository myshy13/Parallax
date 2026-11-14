#pragma once
#include "Lighting/Lighting.hpp"
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

  // no copy operations
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  void submit(const DrawCommand &command) { queue.push_back(command); }
  void flush(Camera3D &camera);

  // ==== getters ==== //
  const Shader &getShader() const { return shader; }
  const Shader &getGroundShader() const { return groundShader; }

  const Texture &getRenderTexture() const { return target.texture; };

private:
  void setRenderTexture();
  static constexpr float RENDER_SCALE = GLSL_VERSION == 100 ? 2.0f : 1.5f;
  RenderTexture2D target{};
  std::vector<DrawCommand> queue{};

  Shader shader;       // lit shader for the ship
  Shader groundShader; // makes gridlines

  Shader loadInstancedShader(const char *name);

  Lighting lighting;
  int sunLight{-1};
};