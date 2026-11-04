# AI Development Log — ROTATORZ
---

## 1. AI Tools Used

| Tool | Purpose | Frequency |
|------|---------|-----------|
| **OpenCode (Nemotron 3 Ultra)** | debugging, refactoring, architecture advice, documentation | Daily |
| **DeepSeek** | Algorithm discussion, C++ patterns, SDL3 API clarification | Occasional |

---

## 2. Categories of Use

| Category | Description | Examples |
|----------|-------------|----------|
| **Code Generation** | Boilerplate, system scaffolding, helper functions | Game loop template, `ObjectPool<T>`, rotation tween, grid wave spawner |
| **Debugging** | Error interpretation, crash analysis, logic bugs | SDL_Init failure handling, render texture setup, orientation-gated collision, gitignore for the initial documentation commit |
| **Refactoring** | Modern C++ improvements, RAII, smart pointers, const-correctness | Converting raw pointers to `unique_ptr`, adding move semantics |
| **Documentation** | README, project.md structure, AI log, code comments | This file, README.md, project.md skeleton |
| **Architecture Discussion** | Pattern selection, class design, data flow | OOP/inheritance vs ECS, delta-time vs fixed timestep, world/window separation |
---

## 3. Example Prompts

**Movement and rotation**
> "Rework the player so movement is horizontal in world space only, locked near the bottom edge, with a smooth rotation tween between portrait (0°) and landscape (90°) instead of an instant snap."

**Enemy waves and fire**
> "Replace the random enemy spawner with a 6×4 grid formation whose rows alternate killable orientation, and let only the front-line enemy in each column fire downward at the player."

**World and rendering**
> "Separate the world size from the window size: render a 480×800 world to an off-screen texture, then scale and centre it inside a 1280×720 window using the rotated AABB so the whole playfield stays visible."

**Collision and orientation gating**
> "Enemy bullets should only damage the player, player bullets should only kill enemies when the current view orientation matches the enemy's colour, and enemy contact should cost a life with a short respawn invincibility."

---

## 4. Verification Process

For each AI-generated output:

1. **Compile Test** — Paste into project, build in Visual Studio (Debug x64). Fix any compile errors.
2. **Runtime Test** — Run the game, exercise the feature. Verify behaviour matches intent.
3. **Code Review** — Read the generated code. Check for:
   - SDL3 API correctness (SDL3 ≠ SDL2 — no `SDL_RendererFlip` enum, different init flags)
   - Memory safety (no leaks, double-free, use-after-move)
   - Performance (avoid allocations in hot paths, prefer stack/pool)
4. **Integration** — Adapt to project architecture (naming, namespaces, include paths, coding style).
5. **Refinement** — Add error handling, logging, comments, unit-test-style assertions.
6. **Commit** — Meaningful message: "Add delta-time GameLoop with frame clamping", not "AI code".

> **Repository Evidence:** Git history shows iterative commits refining initial AI output (e.g., `StateMachine` went through 4 commits: scaffold → enter/exit → push/pop → transition guards).

---

## 5. Ownership & Independent Judgement

- **All architectural decisions** (state machine vs. stack, pool design, rotation approach) were made by me after discussing trade-offs with AI.
- **AI never dictated scope** — I chose which features to build and in what order.
- **Bugs introduced by AI:** an early orientation check was inverted (landscape enemies wrongly vulnerable at 0°), and the off-screen texture initially matched the window size instead of the world size, which broke the 90° fit. Both were identified through review and corrected.
- **Performance choices:** object pools for bullets and enemies, a single off-screen render texture, and no per-frame heap allocation in update/render paths.
- **Creative decisions** (rotation mechanic, orientation-locked enemies, visual style) are entirely original.

---

## 6. Implementation Summary

### Core Architecture
- OOP/inheritance design: `GameObject` base class with virtual `Update`, `Render`, and `GetBounds`, derived by `Player`, `Enemy`, and `Bullet`.
- `ObjectPool<T>` template with auto-expansion used for player bullets, enemy bullets, and enemies.
- `Game` owns the window, renderer, off-screen world texture, input manager, pools, and game state.
- Delta-time loop via `SDL_GetPerformanceCounter`, with frame delta clamped to 50 ms to prevent tunnelling after stalls.

### World and Rendering
- Fixed 480×800 world rendered into an off-screen `SDL_Texture`, separate from the 1280×720 window.
- `SDL_RenderTextureRotated` presents the world at `currentAngle`, with the destination rect scaled and centred.
- Scale is derived from the rotated world's axis-aligned bounding box (`|cos|`/`|sin|`), so the whole playfield fits at both 0° and 90°.
- Reference grid and a corner marker make the current rotation readable.

### Rotation Mechanic
- `Q` toggles between the two supported orientations: portrait (0°) and landscape (90° clockwise).
- `currentAngle` tweens toward `targetAngle` at `kRotationSpeed` (300°/s).
- Rotation is purely visual: movement, bullet direction, and collision stay in world space; input is never remapped.

### Player
- A/D (or arrow keys) move horizontally along world X; Y is locked near the bottom edge via `kPlayerY`.
- The ship is clamped inside a 15 px margin band so it never leaves the visible playfield.
- Left click or Space fires upward at a fixed cooldown.
- Three lives with temporary invincibility and a blinking sprite after taking damage.

### Enemies
- Waves spawn as a 6×4 grid, centred horizontally, with rows alternating orientation.
- Orange enemies are vulnerable in portrait (0°); green enemies are vulnerable in landscape (90°).
- The formation descends steadily; the lowest enemy in each column is the front line and fires downward.
- Orientation-gated collision: bullets only kill an enemy when the current view matches its colour.

### Game Flow
- Score scales with the wave number.
- Clearing the field waits two seconds, then spawns the next wave and increments the wave counter.
- Losing all lives enters the Game Over state, with a name-entry field ready for high-score persistence.
- Player bullets and enemy bullets use separate pools and colours (yellow vs red).

### Documentation
- `README.md`: controls, gameplay overview, and a "What Changed" comparison table.
- `project.md`: architecture, rotation system, enemy behaviour, and mechanics notes.
- `ai-development-log.md`: this implementation record.

---