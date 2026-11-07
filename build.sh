#!/bin/sh
# Configures (Ninja, via CMakePresets.json) and builds the native game into build/.
set -e
cd "$(dirname "$0")"
cmake --preset native
cmake --build build
