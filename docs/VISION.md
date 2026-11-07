# Parallax: Vision (parking lot)

Everything here is deliberately NOT being built yet. Writing an idea here counts as dealing with it. I do not read this file while working on the current stage. I revisit it only when a stage has shipped.

## The long-term idea

A realistic, procedurally generated space game where the player decides what to do. Explore, build or survive, the way Minecraft lets the player choose, but in a believable universe.

## Stage 2 plan: Fly through a generated system (6-8 weeks)

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

First person, from the cockpit.

### Visual direction

Realistic, shader-driven lighting from the star. Planet surfaces come from generated noise in the shader, with atmospheric scattering at the planet's edge.

### Assets

- Planets, star, rings, atmosphere: generated in shaders, no art files.
- Starfield and backdrop: procedural.
- Asteroids: noise-displaced spheres, instanced.
- Ship: cockpit frame and HUD only.
- HUD: flat shapes and a font.

### Build order

If time runs out, I cut from the bottom.

1. A lit sphere and a camera I can move around
2. Flight controls
3. One noise-shaded planet
4. The star, and lighting from it
5. The full seeded system: planets, moons, rings, belt
6. Atmospheres
7. Cockpit and HUD
8. Polish

### Learning goals

GLSL shader work: atmospheric scattering, normal-mapped lighting, a glow or bloom effect for the star.

## Parked ideas

### Leaving the ship

- Exterior ship model (replaces the cockpit-only view)
- Spacewalk: a free-floating player body with a jetpack, no gravity
- Reference frame handling (the player moves relative to the drifting ship)
- Re-entering the ship
- Later: walking anywhere beyond the flat crash site (needs landing terrain and gravity)
- Gravity per planet, from the seed (mass and radius); Stage 1 only has a tunable value

### Worlds

- Landing on planets with chunked cube-sphere terrain
- Level of detail from orbit down to ground
- Flat terrain near the ground, switching to a low-poly planet above about 1000 m altitude
- Biomes, water, weather, day/night from the surface
- Cloud layers and better atmospheres
- Multiple star systems, then a galaxy map
- Procedural planet names and a discovery log

### Ship and flight

- Repairing the crashed ship and taking off, as a choice rather than an objective
- Third-person camera and ship customization
- Docking, stations, landing gear
- A warp or jump drive

### Sandbox layers (each is its own stage)

- Building: placing structures on planets or in orbit
- Survival: resources, energy, oxygen, hazards
- Crafting and upgrades
- Mining asteroids and surface deposits
- Exploration goals: scanning, cataloguing

### Life and content

- Creatures
- Ruins, anomalies, points of interest
- Trading or NPCs
- Combat, if it ever feels right

### Technology

- Saving and loading
- Sound and music
- Seed sharing UI
- Web build polish
- Multiplayer (the sandboxNetwork code could help)

## Rule

Nothing moves out of this file until the stage in GOAL.md that precedes it has shipped.
