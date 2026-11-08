# Class backlog

Changes to existing code to make when a class is added. Only what isn't obvious from the class itself.
Remove an entry once done, and a class's heading once it's empty.

## World

- Move `gravity` from `Engine` to `World`: it changes per world. `Player::update` gets it from the world, not `Engine`.
- Move the ground (`groundPlane`, `GROUND_PLANE_SIZE` and its unload in `~Engine`) from `Engine` to `World`.
