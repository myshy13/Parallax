#pragma once

#include "Terrain/Noise/noise.hpp"
#include "World/world.hpp"
#include <cstdint>
#include <raylib.h>
#include <unordered_map>
#include <vector>

struct Chunk {
  int cx, cz;
  std::vector<int64_t> heightPoints;
};

constexpr int CHUNK_SIZE = 16;
constexpr float VERTEX_SPACING = 0.5f; // raylib units/ metres
constexpr int CHUNK_VIEW_RADIUS = 8;   // generating and viewing chunks

class Terrain {
private:
  int64_t seed;
  const World &world;
  Noise noise;

  std::unordered_map<int64_t, Chunk> cache; // index to a heightpoint

public:
  Terrain(World &world);

  // copy protection
  Terrain(const Terrain &) = delete;
  Terrain &operator=(const Terrain &) = delete;

  float getHeight(int64_t) const;
  Vector3 getPosition(int64_t) const;
  std::vector<const Chunk*> getChunks(Vector3 centre, float radius);
  void update(Vector3 viewpointPos);
};
