# Rendering

Decisions for how rendering is organized in Stage 1, and what I do when it gets slow.

## Units

1 raylib unit = 1 metre. Everything in the game uses this scale.

Two things follow from it:

- raylib's default clip planes are 0.01 to 1000 units, so anything over 1 km away is cut off. Set them with `rlSetClipPlanes` before the first 3D draw.
- raylib positions are 32-bit floats, and they lose precision with distance from the origin:

| Distance from origin | Gap between representable positions |
| --- | --- |
| 1 km | 0.06 mm |
| 100 km | 8 mm |
| Earth radius (6,400 km) | 0.5 m |
| Moon distance (380,000 km) | 32 m |
| 1 AU (150 million km) | 16 km |

Real distances will jitter visibly and won't fit a usable depth range. Not solved yet. Decide before Stage 2's seeded system how big the system really is, and whether world positions need doubles with rendering done relative to the camera.

## Web and native differences

No `#ifdef`s in rendering code. raylib already hides the platform, so the renderer makes the same calls on both.

- Shader folder: `TextFormat("assets/shaders/glsl%i/lit.fs", GLSL_VERSION)`.
- Feature limits on GLSL 100: `if constexpr (GLSL_VERSION == 100)`, when both branches compile on both platforms.
- Platform-only code, such as the main loop if I drop `-sASYNCIFY`: `src/platform.h`, with `src/platform/native.cpp` and `src/platform/web.cpp`. CMake compiles one of them per build. Add this when the main loop has to split, not before.

No virtual backend classes (`RendererSource` and subclasses). Each build only ever has one implementation, so there is nothing to choose at runtime. Revisit only if the platforms stop sharing raylib.

## Renderer class

One plain, non-virtual `Renderer`, added once `main.cpp` gets crowded. It is for organizing code, not for performance.

- It may own the window, because GPU resources must be freed before `CloseWindow()`.
- Input and the game loop stay out of it.
- Build step 1 starts as plain functions, so the class is based on what the code turned out to need.

## Batching

Batching is not a mode the renderer switches on when frames drop. It is a code change I make when measurements show draw calls are the problem.

What raylib does:

- Shapes, text and `DrawCube`-style calls are already batched.
- `DrawMesh` and `DrawModel` are one draw call each.
- `DrawMeshInstanced` draws one mesh many times in a single call.

`Renderer::flush()` draws every batch with `DrawMeshInstanced`, even a single mesh, so every material it draws needs a shader that reads the per-instance transform as a vertex attribute (`instanceTransform`, set on `locs[SHADER_LOC_MATRIX_MODEL]`). The web build relies on `GL_ANGLE_instanced_arrays` for it. Not yet tested on the web: check it on the low-end device.

## Lighting

`Renderer` owns a `Lighting` (`src/Lighting/`). The scene is lit by directional lights, with the sun as light 0, plus one flat ambient colour.

- `Lighting` holds the lights (each has an id, a unit `dir` pointing TO the light, and a colour) and an ambient colour.
- Every shader that is lit is registered once with `Lighting::addShader()`, which looks up that shader's uniform locations. Locations belong to a shader program, so each shader has its own.
- `Renderer::flush()` calls `Lighting::upload()` once per frame, inside `BeginMode3D`, which sets the uniforms on every registered shader.
- The uniforms are `lightDir[]`, `lightColour[]`, `lightCount` and `ambientColour`, and the loop is the same in `shader.fs` and `ground.fs`: ambient plus `max(dot(N, lightDir[i]), 0.0) * lightColour[i]`.
- The ground shader uses a fixed normal of `vec3(0, 1, 0)`. That is only right while the ground plane is flat and its transform has no rotation.
- `MAX_LIGHTS` (4, so the GLSL 100 shaders stay within the 16 fragment uniform vectors WebGL 1 guarantees) lives in `Lighting.hpp` and in every lit shader (both versions). Change them together.
- Normals are `mat3(instanceTransform) * vertexNormal`, which is only right for uniform scale. The player's box is scaled non-uniformly but still lights correctly, because its normals are axis-aligned and the scale is applied before the rotation. Any other mesh scaled non-uniformly needs the inverse transpose.
- Lighting is done in sRGB space, with no gamma correction yet.
- The lit ship shader also adds a Blinn-Phong specular highlight: `H = normalize(L + V)`, then `pow(max(dot(N, H), 0.0), shininess)`, only on the lit side, added after the albedo multiply so it takes the light's colour. `shininess` and `specStrength` are constants in `shader.fs`. It needs `fragPos` (world position, `highp` on GLSL 100) from the vertex shader and `viewPos` (`camera.position`), which `Lighting::upload(viewPos)` sets each frame.
- The ground has no specular yet; revisit it with the noise in build step 4. Its shader has no `viewPos`, so that location is -1 and `SetShaderValue` skips it (raylib 5.5 doesn't log missing uniforms).
- Not done yet: a hemisphere ambient, gamma correction, point lights (a later idea).
- The creator writes the `glsl330` shaders and Claude ports them to `glsl100`.

## When it gets slow

1. Read the frame-time counter in a Release build on the low-end device.
2. Find the cost. Shrink the window: if frame time improves a lot, the cost is in pixels.
3. Fix only that:

| Cost | Fix |
| --- | --- |
| Heavy fragment shaders (noise, atmosphere) | Bake noise into textures, simplify the shader, or render at a lower resolution |
| Too many triangles on distant spheres | Lower-detail meshes for far objects |
| Thousands of separate asteroid draw calls | `DrawMeshInstanced` |
