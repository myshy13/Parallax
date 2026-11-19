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

// ==== day length ==== //
constexpr uint32_t DAY_STREAM_A =
    0x85ebca6bu; // each stream must differ, or the average stops being a bell
constexpr uint32_t DAY_STREAM_B = 0xc2b2ae35u;
constexpr uint32_t DAY_STREAM_C = 0x27d4eb2fu;
constexpr float UNIT_FLOAT_SCALE = 1.0f / 16777216.0f; // 1 / 2^24
constexpr float MIN_DAY_MINUTES = 10.0f;
constexpr float DAY_RANGE_MINUTES = 20.0f; // min + range = 30, mean = 20
constexpr float SECONDS_PER_MINUTE = 60.0f;

// Maps a hash to [0, 1); the top 24 bits fit a float exactly, so 1.0 can't
// appear.
inline float hashToUnit(uint32_t hash) {
  return static_cast<float>(hash >> 8) * UNIT_FLOAT_SCALE;
}

// Bell-shaped day length in seconds (10 to 30 minutes, mean 20) from the
// average of three hashes.
inline float hashDayLength(uint32_t seed) {
  float a = hashToUnit(mixBits(seed ^ DAY_STREAM_A));
  float b = hashToUnit(mixBits(seed ^ DAY_STREAM_B));
  float c = hashToUnit(mixBits(seed ^ DAY_STREAM_C));
  float average = (a + b + c) / 3.0f;

  return (MIN_DAY_MINUTES + DAY_RANGE_MINUTES * average) * SECONDS_PER_MINUTE;
}
