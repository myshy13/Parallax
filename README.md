# Parallax

C++20 and raylib 5.5, built with CMake 3.20+. raylib is downloaded on the first configure.

## Native build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/parallax
```

## Web build

Requires the Emscripten toolchain.

```sh
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release # for optimisations
cmake --build build-web
emrun build-web/dist/parallax.html
```
