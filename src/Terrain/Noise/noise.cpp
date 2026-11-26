#include "Terrain/Noise/noise.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace {

constexpr int GRADIENT_COUNT = 8;
constexpr float GRADIENTS[GRADIENT_COUNT][2] = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

// Smoothstep with zero slope at both ends: 6t^5 - 15t^4 + 10t^3.
inline float fade(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

inline float mix(float a, float b, float t) { return a + (b - a) * t; }

// Dot of the hashed corner gradient with the corner-to-point offset.
inline float gradDot(int hash, float dx, float dy) {
  const float *g = GRADIENTS[hash % GRADIENT_COUNT];
  return g[0] * dx + g[1] * dy;
}

} // namespace

// ==== construction ==== //
Noise::Noise(int64_t seed) {
  std::iota(perm.begin(), perm.begin() + PERM_SIZE, 0);

  std::mt19937 rng(static_cast<uint32_t>(seed));
  std::shuffle(perm.begin(), perm.begin() + PERM_SIZE, rng);

  std::copy(perm.begin(), perm.begin() + PERM_SIZE, perm.begin() + PERM_SIZE);
}

// ==== sampling ==== //
float Noise::sample(float x, float y) const {
  float floorX = std::floor(x); // floor, not an int cast, so negatives work
  float floorY = std::floor(y);
  float fracX  = x - floorX;
  float fracY  = y - floorY;
  int cellX    = static_cast<int>(floorX) & PERM_MASK;
  int cellY    = static_cast<int>(floorY) & PERM_MASK;

  int hashBL = perm[perm[cellX] + cellY];
  int hashBR = perm[perm[cellX + 1] + cellY];
  int hashTL = perm[perm[cellX] + cellY + 1];
  int hashTR = perm[perm[cellX + 1] + cellY + 1];

  float bottom = mix(gradDot(hashBL, fracX, fracY),
                     gradDot(hashBR, fracX - 1, fracY), fade(fracX));
  float top    = mix(gradDot(hashTL, fracX, fracY - 1),
                     gradDot(hashTR, fracX - 1, fracY - 1), fade(fracX));

  return mix(bottom, top, fade(fracY));
}

float Noise::fbm(float x, float y, int octaves) const {
  float sum       = 0.0f;
  float amplitude = 1.0f;
  float frequency = 1.0f;
  float total     = 0.0f; // sum of amplitudes, to normalise by

  for (int i = 0; i < octaves; ++i) {
    sum += amplitude * sample(x * frequency, y * frequency);
    total += amplitude;
    amplitude *= FBM_PERSISTENCE;
    frequency *= FBM_LACUNARITY;
  }

  return total > 0.0f ? sum / total : 0.0f;
}
