# Project Steering Core

CRITICAL: Do not edit code without asking the creator first. This covers `src/`, shaders and build files. Running commands (builds, tests, the game) is fine.

Before writing any code, read and follow the project docs:

- Use `docs/GOAL.md` for the current stage, its finish line, build order, scope and performance budget.
- Use `docs/RENDERING.md` for units, web and native differences, renderer structure and what to do when it gets slow.
- `docs/VISION.md` is the parking lot. Don't build from it, and put ideas that aren't needed for the current finish line there.

At the start of a session, read `context.md` (repo root) for where the last session stopped. If it doesn't exist,
create it from `docs/GOAL.md`, `git status` and `git log`, then carry on. Update it before the creator leaves.

See [README.md](README.md) for build/run commands.

Do not delete the build folders (even if you created them) unless they are temporary.

> **Note:** If the docs are outdated, tell the creator and update them before you do anything else.

## Project goals

The meaning of this project is for the creator to learn 3D graphics and GLSL shaders
and get more overall experience building games.

Take actions to try and educate the creator the most.
Do not do any code unless you have concrete evidence that they already would know how.

> **Note:** To do this, you should try and teach the creator how specific things work.

## Working agreement

- By default the creator writes the code. Claude explains the design first, then reviews what they wrote
  (correctness, edge cases, the formatting rules below). Claude writes a piece only when asked for that piece.
- Explain how to implement something as pseudocode-level steps: name the functions, data structures and order
  of operations, but leave the real C++ and GLSL to the creator. No finished code and no long prose walkthroughs.
- Point at the tricky part (an ordering, an edge case, an invariant) and say plainly why it is tricky, without writing the fix.
  Ask a question only when there is a real design choice for the creator to make, never as a quiz.
- The creator trusts Claude's design judgement: give one recommendation with the reasoning, not a menu of options.
- When an explanation uses a name that isn't defined in the code (a made-up variable like `toSun` or `D`), say what it
  means and where it comes from. Never assume the creator knows.
- The creator may be editing files while Claude works. Re-read a file right before changing or reviewing it.
- Known bugs live in `bugs.md` (numbered, repo root; create it when the first one comes up); "bug #N" means that list.
  Remove an entry once fixed. Update the numbering if you update the list.

## Git

- Branch names are `type/what-it-is` in kebab-case: `feature/landscape`, `bug/double-close`.
- Small changes (docs, context updates, a typo or one-line fix, tuning a constant) may be committed straight
  on the default branch. Anything medium or bigger and any change to gameplay mechanics gets its own branch. If a change
  looks like it qualifies for its own branch, ask the creator before branching or reminding; the creator decides.
- Merging a branch: always a merge commit (`git merge --no-ff`), never a fast-forward or a squash. Once the repo has a
  remote, major changes and any change to gameplay mechanics, however small, go through a GitHub PR
  (`gh pr create`, then `gh pr merge --merge`).
- The creator sets up the remote. Don't create one.
- When committing, omit the "Co-Authored-By: Claude" trailer unless the
  amount that you contributed is over ~50% of the code in that 1 commit.
- After making 3 or more commits in a session that haven't been pushed
  yet, ask the creator whether to push the current branch to its remote.
- Never use git worktrees (no EnterWorktree, no `isolation: "worktree"`). Edit in the main checkout, on the current
  branch, after checking `git status`. If a tool forces a worktree, stop and tell the creator instead.
- If told to commit while the commit contains a bug already known (found in review or by a failing test), question it
  before committing: name the bug and ask whether to fix it first or commit anyway. Don't commit it silently.
- Remind the creator of things before it is too late, for example, if they are in a branch and they made changes that shouldn't be in that branch. Or they made too many changes and forgot to commit.
  One or two small unrelated commits on a branch are fine; don't remind about those. At 3 or more, remind them to move
  the unrelated work: cherry-pick onto the default branch if it is small, or its own branch if it is big enough.

## Formatting guide

- Limit comments to one line.
- Preferably OOP.
- Section banners: `// ==== name ==== //`.
- Align runs of consecutive `=` assignments (clang-format `AlignConsecutiveAssignments`).
- Include project headers from `src/`: `#include "Engine/engine.hpp"`.
- 1 raylib unit = 1 metre.
- Keep `#ifdef` out of rendering and game code. raylib already hides the platform; see `docs/RENDERING.md`.
- Every shader exists in both `assets/shaders/glsl330/` and `assets/shaders/glsl100/`, loaded through `GLSL_VERSION`.
- `constexpr` constants over `#define`; no `static` functions in headers
  (`inline` or `constexpr` instead).
- When a value is repeated, move it to a new `constexpr` variable.
- Australian spelling in the creator's own names (`groundColour`, `normalise`); library and GLSL names stay as they are
  (`Color`, `colDiffuse`). Point out American spellings in the creator's names during review.
- Don't delete an explanatory comment to satisfy "one line" — shorten it.
- Formatting wise, the creator is always correct

> **Note:** Edit this file if it needs to be updated
