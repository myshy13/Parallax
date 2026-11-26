# Terrain (chunked heightfield)

Branch plan for the chunked terrain. Delete this file once the branch is merged and the design lives in the code.

> `GOAL.md` lists chunked terrain as out of scope for Stage 1 (parked for Stage 5). Decide whether it is pulled into Stage 1, and update `GOAL.md` before building.

## Requirements

- Chunked.
- `getChunks` returns the chunks near a `Vector3` position within a radius.
- Every height point id can be asked of `Terrain`, and returns a float in metres from 0. It can be below 0 (water later).
- Each `Chunk` holds a vector of height point ids; the heights are asked of `Terrain`.

## Design

A height point id is a global lattice coordinate, not an index into storage. Heights come from a pure function of `(x, z)`, so nothing is stored per point.

- `HeightPointId` is an `int64_t` packing `(ix, iz)`: `ix` in the top 32 bits, `iz` in the bottom 32. `inline` helpers `packId(ix, iz)` and `unpackId(id)`.
- World position: `x = ix * POINT_SPACING_M`, `z = iz * POINT_SPACING_M`.
- Height: `noise.fbm(x * FREQ, z * FREQ, OCTAVES) * HEIGHT_SCALE_M`. Signed, never clamped, so it goes below 0 on its own. Optional `SEA_LEVEL_OFFSET_M` later.
- Chunks are a grid on X and Z (Y is height). Chunk `(cx, cz)` starts at `ix0 = cx * (POINTS_PER_SIDE - 1)`, and `iz0` likewise for `cz`.

### Constants (starting values)

| Name | Value | Meaning |
|------|-------|---------|
| `CHUNK_SIZE_M` | 32 | chunk edge length in metres |
| `POINT_SPACING_M` | 2 | gap between height points |
| `POINTS_PER_SIDE` | `CHUNK_SIZE_M / POINT_SPACING_M + 1` = 17 | points along one chunk edge |
| `HEIGHT_SCALE_M`, `FREQ`, `OCTAVES` | tune in play | height amplitude, noise scale per metre, fbm layers (keep low for web) |

### Classes

```
struct Chunk
  cx, cz                         // grid coordinate
  vector<HeightPointId> points   // row-major, POINTS_PER_SIDE²

class Terrain
  Terrain(int64_t seed)          // builds Noise; no World reference
  float   getHeight(HeightPointId) const        // metres, can be < 0
  Vector3 getPosition(HeightPointId) const      // x, z from the id, y from getHeight
  vector<const Chunk*> getChunks(Vector3 centre, float radius)
  private: Noise noise; unordered_map<int64_t, Chunk> cache   // key = packId(cx, cz)
```

`getPosition` is the one addition to the requirements: a mesh builder needs it, because ids alone don't say where a point is.

### `getChunks(centre, radius)`

1. Ignore `centre.y` (heightfield; only X and Z matter). Say so in a comment.
2. Chunk range: `floor((centre.x ± radius) / CHUNK_SIZE_M)`, same for Z.
3. For each `(cx, cz)` in the range, clamp the centre into the chunk's rectangle and keep the chunk only if that distance <= `radius` (chunks that touch the circle).
4. Look the chunk up in `cache`; if missing, build `points` with `packId(ix0 + i, iz0 + j)` for `i`, `j` in `0..POINTS_PER_SIDE-1`. Return a pointer to it.

## Tricky parts

- Neighbouring chunks share their border points with the same ids because ids are global. That stops cracks. Never use chunk-local ids.
- Use `floor`, not an `(int)` cast, for the chunk coordinate (negative coordinates, seam at 0).
- Pointers into an `unordered_map` survive growth but not `erase`. When far chunks get evicted later, nothing may still hold pointers to them.
- Normals at chunk borders need neighbour heights: call `getHeight` on ids outside the chunk.
- `getHeight` runs `fbm` once per point (about 1,200 `sample` calls per 17x17 chunk at 4 octaves). Build chunks once and cache them.

## Steps

1. Update `GOAL.md` for chunked terrain (or decide to defer it).
2. `Terrain` takes `int64_t seed` and drops the `World` reference and the `unordered_map<Vector2, ...>` (no `Vector2` hash exists, and it isn't needed).
3. Add the constants, `HeightPointId`, `packId` and `unpackId`.
4. `getHeight` and `getPosition`.
5. `Chunk` and the cache; build a chunk from its coordinate.
6. `getChunks` with the range and circle test.
7. Check: the same seed gives the same height; points on a shared border match between two chunks; negative coordinates work; some heights are below 0.
8. `World` owns a `Terrain` built from `getSeed()`. Meshing and replacing the flat ground plane is a later step on this branch or its own.
9. Check `docs/BACKLOG.md` for entries due when `Terrain` is added.

## After this branch

Each stage below is a small branch of its own, in this order. Mesh and draw must come first, because you can't judge anything else until the ground is on screen. Each has a one-line check to finish it. Test frame time in a Release build after every one (GOAL: 30 fps at 720p on the Chromebook).

### 1. Mesh and draw (`feature/terrain-mesh`)

Check: the flat ground plane is gone and you walk over visible hills, with no cracks between chunks.

- Per chunk, build one raylib `Mesh`: one vertex per height point (289 of them), position from `getPosition(id)`, two triangles per grid square (512 triangles), `UploadMesh` once. Draw it through a `DrawCommand` with `MatrixIdentity()`, like the ground plane today. Chunk vertices are already in world coordinates, so no per-chunk transform is needed.
- Normals: central differences. For a point, `normal = normalise(h(x-1) - h(x+1), 2 * POINT_SPACING_M, h(z-1) - h(z+1))`, where `h` is `getHeight` on the neighbouring ids. Neighbour ids can be outside the chunk, which is why heights aren't stored in `Chunk`. This is what keeps lighting seamless across borders.
- Keep a `Chunk` to `Mesh` map. Build meshes for chunks that appear in `getChunks` and `UnloadMesh` the ones that leave. Cap how many meshes you build per frame (a few) so walking doesn't hitch.
- Tricky: split every square along the same diagonal in every chunk, and use the same split in collision (stage 2). If the two disagree, the player floats or sinks relative to what they see.

### 2. Collisions (`feature/terrain-collision`)

Check: you walk up and down slopes without sinking, floating or jittering, and you slide off slopes that are too steep.

Today the ground is one big `OBB` at y = 0 in `World::colliders`, and `Player::resolveCollisions` tests the body against every `OBB` with `GetContactOBB`. A heightfield doesn't fit boxes, so keep the `OBB` colliders for the ship and handle terrain separately.

- Add `float Terrain::getHeightAt(float x, float z)`. It finds the grid square the point is in, reads the four corner heights with `getHeight`, and interpolates across the same triangle split as the mesh. It also gives the slope normal of that triangle.
- In `Player`, after the `OBB` pass: take the player's feet position, get `groundY` from `getHeightAt`. If `feet.y < groundY`, move up to `groundY`, zero `vel.y` and set `grounded`. This replaces the ground box, which you delete.
- Max slope: `grounded` only counts if `normal.y >= cos(MAX_SLOPE)`. On a steeper slope the player slides: remove the velocity into the slope and let gravity pull along it. Pick `MAX_SLOPE` in play (start near 45 degrees).
- The existing downhill snap (`GROUND_SNAP`) keeps working as long as terrain contact counts as grounded.
- Tricky: a single point query can't tunnel through terrain, but the player is a box. Test the foot centre first, and only move to the four bottom corners if you see the body clip into a hillside. Don't test every corner by default.
- Tricky: when terrain is loaded in chunks, `getHeightAt` must work for any `(x, z)`, loaded or not. It does, because the height is a pure function and doesn't need a chunk to exist.
- The ship must sit on the terrain. Set its Y from the terrain at its position, then flatten the terrain near the crash site with a mask (a `smoothstep` of distance from the ship that blends the height toward the ship's level), so it doesn't poke through a hill or hang in the air. Apply the mask inside `Terrain`, in one place, so mesh, collision and shading all agree.

### 3. Shading (`feature/terrain-shading`)

Check: the hills read as 3D under the moving sun, and the colour varies by slope or height, not just noise.

- The ground shader has a hardcoded `groundNormal = vec3(0, 1, 0)` and a `fragWorldXZ` varying. Replace the constant with the per-vertex normal from stage 1 (add a `vertexNormal` attribute and a varying). The diffuse term then uses the real surface normal. Do both `glsl330` and `glsl100`.
- Colour: keep your noise mix of `dustDark` and `dustLight`, and also mix toward a rock colour by slope (`1 - normal.y`) and optionally by height. Cheap and makes hills look like terrain.
- If frame time suffers, bake the colour per vertex on the CPU from slope and height (GOAL says to bake or approximate surface noise) and drop per-pixel noise from the `glsl100` version.
- Note that the ground shader has 16 `sin` per pixel (listed in `context.md`). Terrain makes that visible, so measure.

### 4. Hide the edge (`feature/terrain-fog`, GOAL step 6)

Check: you can't see chunks pop in at the edge.

- The `getChunks` radius sets the view distance. Add fog toward `horizonColour` (the colour the sky already uses at the horizon) that reaches full strength just before the radius. Then chunks appear and disappear inside the fog. Needs uniforms in the ground and ship shaders for both GLSL versions, plus `Lighting::upload`.
- Choose the radius and the fog distance together. A larger radius is more chunks and more draws. Pick the smallest that looks right.

### 5. Tune and measure

- Tune `HEIGHT_SCALE_M`, `FREQ` and `OCTAVES` in play. Plain noise gives even rolling hills. For variety, try raising the noise to a power (flat plains with sharp hills) or a ridged variant, and `fbm` octaves per web or native tier (the glsl330/glsl100 rule for quality tiers also applies to terrain settings).
- Measure frame time on the lowest-spec machine. If it is slow: raise `POINT_SPACING_M`, drop octaves, shrink the radius, or reduce chunk builds per frame.

### Later (park in `VISION.md`, not for this stage)

- Water at y = 0 for heights below 0.
- LOD (lower-resolution chunks in the distance) and chunk eviction by distance.
- Large-world precision: float coordinates get coarse far from the origin.
- Cube-sphere terrain for planets (GOAL Stage 5).

### Housekeeping

- Delete `GROUND_PLANE_SIZE`, `GROUND_THICKNESS`, the ground model and the ground box from `World`.
- Add a Terrain section to `docs/RENDERING.md`, and check `docs/BACKLOG.md`.
- Update `context.md` before you stop each session, and delete this file when the design lives in the code.

## Open question

Does the radius mean chunks that touch the circle (recommended, no holes at the edge of view) or chunks whose centre is inside it?
