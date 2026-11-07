# Parallax

Parallax is a singleplayer, procedurally generated space game. The player starts stranded on a planet, beside their crashed ship. Later stages open that up into a single star system built from a seed: a star, planets, moons, rings and an asteroid belt, rendered realistically. The same seed always produces the same system.

This is also a learning project. Learning is a goal, not a side effect.

## Design principle

The game doesn't tell the player what to be. There is no forced objective. Long term it's a sandbox where the player chooses: explore, build, survive, or something different. The crash is the setting, not a mission: repairing the ship is something the player may choose to do, never something they must. Stage 1 delivers only the foundation under that: standing in a believable world.

## Rules for finishing this project

- Every stage has a one-sentence finish line and a time box.
- If a time box runs out, I shrink the scope. I do not move the deadline.
- This file sets direction and limits, not details. Tuning values, small features and how something is built are decided in the moment, as long as they serve the finish line and fit the time box and the performance budget.
- Anything that doesn't serve the current finish line goes into VISION.md.
- Placeholder assets are fine until the finish line is met. Art polish is its own stage.
- Asset budget: one ship, a few materials, no more than a day per asset.

## Stage 1: The crash site (2-3 weeks)

Finish line:

> I can walk around a crashed ship on a lit alien surface under a believable sky.

The crash site is its own scene. There is no way to leave it yet.

### What's in the scene

- Ground, coloured by noise
- One crashed ship
- A sun that lights the scene
- A sky
- Something that hides the edge of the ground (fog, low hills or similar)

### Player

First person, on foot: walking, looking around and jumping. Gravity is a value, not a hardcoded Earth constant, so the alien world can feel different. Feel (speed, eye height, jump height, gravity strength) is tuned in play. No flying.

The player's viewpoint is kept separate from the movement code, so it can sit in a cockpit later without a rewrite.

### Visual direction

Realistic, shader-driven lighting from the sun.

### Assets

- Ground and sky: generated, no art files.
- Ship: a placeholder (e.g. a few stretched, tilted cubes). A real model is art polish.
- HUD: debug information only (FPS and frame time at minimum).
- Textures that do need files are produced by reproducible scripts checked into the repo, not hand-painted.
- Free CC0 sources (Kenney, Quaternius, Poly Haven, ambientCG) are fine; I check each license.

### Build order

Something walkable exists at every point. The order is a guide: a small piece can go in early when it fits the work at hand. If time runs out, I cut from the bottom.

1. Ground, a first-person camera and the frame-time counter
2. The placeholder ship
3. Lighting from the sun (1-week cap for steps 1-3; this is also my 3D warm-up: matrices, depth testing, lighting)
4. Noise on the ground colour
5. The sky
6. Hiding the ground's edge
7. Not walking through the ship
8. Jumping and gravity
9. Polish

### Learning goals

- 3D fundamentals: model/view/projection matrices, depth testing, diffuse and specular lighting (learned during build steps 1-3)
- GLSL shader work: noise-based surfaces and a sky
- Simple physics: velocity, acceleration and a fixed timestep

## Tech

- C++20, raylib 5.5, CMake
- Custom shaders; rlgl only where raylib doesn't expose what's needed
- One star system per seed. No switching systems.

### Performance budget

- Target: 30 fps at 720p on a mid-range Chromebook (integrated GPU), running the web build.
- The web build is a core deliverable.
- Test on a real low-end device (or the lowest-spec machine I can get) at the end of every build-order step, not just at the end.
- Measure before optimising: keep an FPS and frame-time counter in the HUD from step 1. Only trust numbers from Release builds.
- Expensive effects (surface noise, atmosphere) are baked or approximated; no per-pixel noise evaluation in the main render path if avoidable.
- If a step breaks the budget, I simplify that step before moving on.

## Out of scope for Stage 1

Flying, leaving the planet, the seeded star system, curved or chunked terrain, repairing the ship, building, survival, crafting, combat, resources, creatures, saving, multiple systems, multiplayer. All parked in VISION.md.

## Later stages (not committed)

- Stage 2: fly through a generated system (6-8 weeks; plan in VISION.md). Its own scene, not yet connected to the crash site.
- Stage 3: richer planets, better atmospheres, cloud layers
- Stage 4: leaving the ship in space (exterior model, spacewalk with no gravity)
- Stage 5: connect the ground to space: land on one planet using chunked cube-sphere terrain (reuse code from sandboxNetwork). Learning goals: chunking, LOD, large-world precision.
- Stage 6+: the sandbox layer, one activity at a time, each as its own finishable stage

## Definition of done for the whole project

A tagged release, a README with a short demo clip, and a line at the top saying "Complete: learning goals met." Then I archive the repo.
