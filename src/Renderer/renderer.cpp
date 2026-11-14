#include "renderer.hpp"
#include "Lighting/Lighting.hpp"
#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <vector>

Shader Renderer::loadInstancedShader(const char *name) {
  const char *vsPath =
      TextFormat("assets/shaders/glsl%i/%s/%s.vs", GLSL_VERSION, name, name);
  const char *fsPath =
      TextFormat("assets/shaders/glsl%i/%s/%s.fs", GLSL_VERSION, name, name);
  Shader s = LoadShader(vsPath, fsPath);

  s.locs[SHADER_LOC_MATRIX_MODEL] =
      GetShaderLocationAttrib(s, "instanceTransform");
  if (s.locs[SHADER_LOC_MATRIX_MODEL] == -1) {
    TraceLog(LOG_ERROR, "Shader %s location = -1", name);
  }
  return s;
}

static_assert(GLSL_VERSION == 330 || GLSL_VERSION == 100,
              "Invalid glsl version");
Renderer::Renderer()
    : shader(loadInstancedShader("shader")),
      groundShader(loadInstancedShader("ground")), lighting() {

  setRenderTexture();

  lighting.addShader(shader);
  lighting.addShader(groundShader);

  sunLight = lighting.createLight({3, 5, -4}, {255, 238, 222, 255});
}

Renderer::~Renderer() {
  UnloadShader(shader);
  UnloadShader(groundShader);
  UnloadRenderTexture(target);
}

void Renderer::flush(Camera3D &camera) {
  if (IsWindowResized() && GetScreenHeight() > 0 && GetScreenWidth() > 0) {
    UnloadRenderTexture(target);
    setRenderTexture();
  }
  // ==== draw ==== //
  std::vector<Batch> batches;

  // ==== build batches ==== //
  for (const DrawCommand &command : queue) {
    if (!command.mesh || !command.material)
      continue;

    // check it doesn't already exist yet
    auto it =
        std::find_if(batches.begin(), batches.end(), [&](const Batch &batch) {
          return batch.mesh == command.mesh &&
                 batch.material == command.material;
        });

    if (it == batches.end()) {
      batches.push_back({command.mesh, command.material, {command.transform}});
    } else {
      it->transforms.push_back(command.transform);
    }
  }

  BeginTextureMode(target);
  ClearBackground(BLACK);
  BeginMode3D(camera);
  lighting.upload();
  // ==== render batches ==== //
  for (const Batch &batch : batches) {
    DrawMeshInstanced(*batch.mesh, *batch.material, batch.transforms.data(),
                      static_cast<int>(batch.transforms.size()));
  }

  queue.clear();
  EndMode3D();
  EndTextureMode();
}

void Renderer::setRenderTexture() {
  int width = GetScreenWidth() * RENDER_SCALE;
  int height = GetScreenHeight() * RENDER_SCALE;
  target = LoadRenderTexture(width, height);
  SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}
