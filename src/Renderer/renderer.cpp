#include "renderer.hpp"
#include <raylib.h>
#include <raymath.h>
#include <vector>

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
  for (const Batch& batch : batches) {
    DrawMeshInstanced(*batch.mesh, *batch.material, batch.transforms.data(), static_cast<int>(batch.transforms.size()));
  }

  queue.clear();
}