# Parallax

Parallax is a singleplayer, procedurally generated space game. The player flies a ship through a single star system built from a seed: a star, planets, moons, rings and an asteroid belt, rendered realistically. The same seed always produces the same system.

This is also a learning project. Learning is a goal, not a side effect.

## Design principle

The game doesn't tell the player what to be. There is no forced objective. Long term it's a sandbox where the player chooses: explore, build, survive, or something different. Stage 1 delivers only the foundation under that: flying through a believable world.

## Rules for finishing this project

- Every stage has a one-sentence finish line and a time box.
- If a time box runs out, I shrink the scope. I do not move the deadline.
- Anything not needed for the current finish line goes into VISION.md.
- Placeholder assets are fine until the finish line is met. Art polish is its own stage.
- Asset budget: one ship, a few materials, no more than a day per asset.

## Stage 1: Fly through a generated system (6-8 weeks)

Finish line:

> I can fly from the star to a planet and back, every planet looks convincingly real up close, and the same seed always gives the same system.

### What gets generated from the seed

- A star with physically plausible color and intensity
- 3-8 planets of different types (rocky, gas giant, ice, ocean) with noise-driven surfaces, atmosphere rims and optional rings
- 0-3 moons per planet
- One asteroid belt
- A starfield backdrop

### Flight

Six-degree-of-freedom Newtonian-style flight with a flight-assist toggle (damps velocity), plus a boost mode so interplanetary distances are practical.

### Camera

First person, from the cockpit. The player's viewpoint is kept separate from the ship in the code, even though in Stage 1 it always sits in the cockpit. That way leaving the ship can be added later without rewriting the flight code.

### Visual direction

Realistic, shader-driven lighting from the star. Planet surfaces come from generated noise in the shader, with atmospheric scattering at the planet's edge.

### Assets

- Planets, star, rings, atmosphere: generated in shaders, no art files.
- Starfield and backdrop: procedural.
- Asteroids: noise-displaced spheres, instanced.
- Ship: cockpit frame and HUD only. No exterior ship model in Stage 1.
- HUD: flat shapes and a font.
- Textures that do need files are produced by reproducible scripts checked into the repo, not hand-painted.
- Free CC0 sources (Kenney, Quaternius, Poly Haven, ambientCG) are fine; I check each license.

### Build order

I work in this order so something flyable exists at every point. If time runs out, I cut from the bottom.

1. A lit sphere and a camera I can move around (1-week cap; this is also my 3D warm-up: matrices, depth testing, lighting)
2. Flight controls
3. One noise-shaded planet
4. The star, and lighting from it
5. The full seeded system: planets, moons, rings, belt
6. Atmospheres
7. Cockpit and HUD
8. Polish

### Learning goals

- 3D fundamentals: model/view/projection matrices, depth testing, diffuse and specular lighting (learned during build step 1)
- GLSL shader work: noise-based surfaces, atmospheric scattering, normal-mapped lighting, a glow or bloom effect for the star

## Tech

- C++20, raylib 5.5, CMake
- Custom shaders; rlgl only where raylib doesn't expose what's needed
- Emscripten web build is a stretch goal
- One star system per seed. No switching systems.

### Performance budget

- Target: 30 fps at 720p on a mid-range Chromebook (integrated GPU), running the web build.
- The web build is a core deliverable, not a stretch goal.
- Test on a real low-end device (or the lowest-spec machine I can get) at the end of every build-order step, not just at the end.
- Measure before optimizing: keep an FPS and frame-time counter in the HUD from step 1.
- Expensive effects (surface noise, atmosphere) are baked or approximated; no per-pixel noise evaluation in the main render path if avoidable.
- If a step breaks the budget, I simplify that step before moving on.

## Out of scope for Stage 1

Landing, leaving the ship, surface terrain, building, survival, crafting, combat, resources, creatures, saving, multiple systems, multiplayer. All parked in VISION.md.

## Later stages (not committed)

- Stage 2: richer planets, better atmospheres, cloud layers
- Stage 3: leaving the ship (exterior model, spacewalk with no gravity)
- Stage 4: land on one planet using chunked cube-sphere terrain (reuse code from sandboxNetwork). Learning goals: chunking, LOD, large-world precision.
- Stage 5+: the sandbox layer, one activity at a time, each as its own finishable stage

## Definition of done for the whole project

A tagged release, a README with a short demo clip, and a line at the top saying "Complete: learning goals met." Then I archive the repo.
