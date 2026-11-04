#pragma once

// only one for now
#include <raylib.h>
#include <stdint.h>
#include <vector>

enum class AssetType : uint8_t { Texture };

struct AssetHandle {
  uint32_t id;
};

class Assets {
public:
  AssetHandle loadAsset(const char *path) {
    textures.push_back(LoadTexture(path));
    return {static_cast<uint32_t>(textures.size() - 1)};
  }

  const Texture2D& getTexture(AssetHandle handle) { return textures[handle.id]; }

private:
  std::vector<Texture2D> textures;
};