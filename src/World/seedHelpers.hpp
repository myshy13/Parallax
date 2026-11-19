#pragma once

#include <cstdint>
#include <raylib.h>

// ==== seed hashing ==== //
constexpr uint32_t HASH_MULTIPLIER = 0x45d9f3bu;
constexpr uint32_t SECOND_STREAM = 0x9e3779b9u; // odd const; keeps y != x
constexpr uint32_t OFFSET_STEPS = 1000; // offsets land in 0..100 in 0.1 steps
constexpr float OFFSET_DIVISOR = 10.0f;

// Scrambles the bits so nearby seeds give unrelated results.
inline uint32_t mixBits(uint32_t x) {
  x ^= x >> 16;
  x *= HASH_MULTIPLIER;
  x ^= x >> 16;
  x *= HASH_MULTIPLIER;
  x ^= x >> 16;
  return x;
}

inline Vector2 hashSeed(uint32_t seed) {
  uint32_t hashX = mixBits(seed);
  uint32_t hashY = mixBits(seed ^ SECOND_STREAM);

  return Vector2{static_cast<float>(hashX % OFFSET_STEPS) / OFFSET_DIVISOR,
                 static_cast<float>(hashY % OFFSET_STEPS) / OFFSET_DIVISOR};
}
