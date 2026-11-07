#include "renderer.hpp"
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

Renderer::Renderer() {
  static_assert(GLSL_VERSION == 330 || GLSL_VERSION == 100,
                "Invalid glsl version");

  // main shader
  shader = loadInstancedShader("shader");
  // ground shader
  groundShader = loadInstancedShader("ground");
}

Renderer::~Renderer() {
  UnloadShader(shader);
  UnloadShader(groundShader);
}

void Renderer::flush() {
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
  // ==== render batches ==== //
  for (const Batch &batch : batches) {
    DrawMeshInstanced(*batch.mesh, *batch.material, batch.transforms.data(),
                      static_cast<int>(batch.transforms.size()));
  }

  queue.clear();
}