#pragma once

#include <array>
#include <cstdint>

// ==== noise tuning ==== //
constexpr int PERM_SIZE            = 256;
constexpr int PERM_MASK            = PERM_SIZE - 1;
constexpr float FBM_LACUNARITY     = 2.0f; // frequency multiplier per octave
constexpr float FBM_PERSISTENCE    = 0.5f; // amplitude multiplier per octave

class Noise {
public:
  explicit Noise(int64_t seed);

  // Smooth noise, roughly -0.7 to 0.7; always 0 on whole-number coordinates.
  float sample(float x, float y) const;

  // Layers of sample() at doubling frequency; same range as sample().
  float fbm(float x, float y, int octaves) const;

private:
  std::array<uint8_t, PERM_SIZE * 2> perm{}; // shuffled 0..255, stored twice
};
