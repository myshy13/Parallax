#include "terrain.hpp"
#include "World/world.hpp"
#include <raylib.h>
#include <vector>

Terrain::Terrain(World &world)
    : seed(world.getSeed()), world(world), noise(world.getSeed()) {}
