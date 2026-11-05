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

**File persistence**
> "Save high scores to a binary scores.dat file (top 10, name + score), load at startup, and show a leaderboard state after Game Over name entry."

**Game Over menu and retry**
> "Add a Game Over menu with 'Retry' (restart immediately) and 'Enter Name' (save score to leaderboard) options, navigated with Up/Down and selected with Space or Return."


**Starfield**
> "Add a parallax starfield background with 160 stars in 3 layers that drift downward and wrap around the world, matching the folk.cpp prototype."

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
- `F11` toggles borderless fullscreen; the destination rect and UI overlay use the live window pixel size, so layout stays centred after the resize.
- Reference grid and a corner marker make the current rotation readable.
- The menu's starfield render scales X and Y independently (`windowWidth / kWorldWidth`, `windowHeight / kWorldHeight`) so the background fills the window correctly at any resolution; only X was scaled previously, which left the lower part of the screen unpopulated in fullscreen.

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
- Orientation-gated collision: a bullet is consumed on contact with any enemy, but only destroys it when the current view matches its colour. Wrong-coloured enemies block shots instead of letting them pass through.

### Bomb & Explosions
- Right-click fires a bomb upward; a 5-second cooldown prevents spam and starts charged.
- The bomb detonates on contact with any enemy, ignoring orientation, and kills every enemy inside an 80 px radius.
- A flat score is awarded per enemy destroyed by the blast.
- The explosion is cosmetic only: a growing ring and shrinking core that fade over 0.4 s.

### Game Flow
- Game starts at the **Main Menu** (Start Game / Leaderboard / Exit) rendered over the starfield background
- Start game resets game state and enters Playing
- Score scales with the wave number
- Clearing the field waits two seconds, then spawns the next wave and increments the wave counter
- Losing all lives enters the Game Over state with a menu: **Retry** (restart immediately) or **Enter Name** (save score to leaderboard)
- **Name entry:** typed characters render inside a bordered text field with a blinking cursor (toggle every 500 ms). Press Enter to save the score and open the Leaderboard
- Entering a name saves the score to `scores.dat` and transitions to the Leaderboard state
- Pressing Space on the leaderboard returns to the Main Menu
- Player bullets and enemy bullets use separate pools and colours (yellow vs red)

### Menu System
- Main menu title rendered as bitmap letters "ROTATORZ" via `DrawText` at scale 5 (previously drew a placeholder digit `0`)
- `Menu` state added to `GameState` enum (Menu / Playing / GameOver / Leaderboard)
- `menuSelected` tracks highlighted option (0=Start Game, 1=Leaderboard, 2=Exit)
- Up/Down/W/S navigate; Space/Return selects
- Start Game calls `ResetGame()` then sets state to `Playing`
- Leaderboard displays top scores from `scores.dat`
- Exit sets `running = false` to close the game
- `ResetGame()` no longer sets `state = GameState::Playing` — each caller manages state transitions
- `HighScoreEntry` struct holds a 16-char name and integer score.
- Top-10 table stored as `HighScoreEntry highScores[kMaxHighScores]` in `Game`.
- `LoadHighScores()` reads `scores.dat` at startup (count + binary entries); starts empty if the file doesn't exist.
- `SaveHighScores()` writes the table back to `scores.dat` whenever a new score qualifies.
- `AddHighScore()` inserts in descending score order, shifting entries down and clamping to `kMaxHighScores`.
- Game Over → name entry → `AddHighScore()` + `SaveHighScores()` → Leaderboard state.
- Leaderboard renders score-proportional bars (gold/silver/bronze for top 3); SDL3_ttf needed for real text.
- Pressing Space on the leaderboard restarts via `ResetGame()`.

### Static Analysis (C6385/C6386)
- All `highScores[]` reads and writes in `AddHighScore()`, `LoadHighScores()`, `SaveHighScores()`, and `RenderLeaderboard()` are guarded with explicit bounds checks against `kMaxHighScores` and `highScoreCount`.
- `AddHighScore()` re-validates `insertAt` immediately before the final write because the analyzer cannot track the variable across the shift loop.
- This suppresses Microsoft Code Analysis warnings C6385 (invalid data access) and C6386 (buffer overrun) without using `#pragma warning(disable)`.


### Documentation
- `README.md`: controls, gameplay overview, and a "What Changed" comparison table.
- `project.md`: architecture, rotation system, enemy behaviour, and mechanics notes.
- `ai-development-log.md`: this implementation record.

### Assets and Audio Pipeline
- Sprites and audio are loaded from an `assets/` folder next to the executable:
  `player.png`, `enemy_portrait.png`, `enemy_landscape.png`, `music.ogg`, `shoot.wav`.
- `Player` and `Enemy` load textures through SDL3_image and fall back to coloured
  rectangles when a file is missing, so the game still runs without assets.
- SDL3_image has no `IMG_Init`/`IMG_Quit` (that was SDL2_image); initialisation
  happens automatically on first load.
- SDL3_mixer 3.x uses the `MIX_*` API rather than the SDL2 `Mix_*` API:
  `MIX_Init`, `MIX_CreateMixerDevice`, `MIX_LoadAudio`, `MIX_CreateTrack`,
  `MIX_PlayTrack`, `MIX_PlayAudio`.
- Audio bug 1: the first implementation called `MIX_CreateMixer()`, which renders
  into a memory buffer and is never heard. It was replaced with
  `MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, ...)`.
- Audio bug 2: music was loaded and assigned to a track but never started. Playback
  now begins with `MIX_PlayTrack` and loops via `MIX_PROP_PLAY_LOOPS_NUMBER = -1`,
  set at play time because `MIX_SetTrackLoops` is overridden when a stopped track starts.
- Audio bug 3: the shoot sound created a track per shot but never played it, leaking
  tracks. It now uses `MIX_PlayAudio`, SDL_mixer's fire-and-forget path.
- Audio is optional: if the mixer or device cannot be created the game logs a message
  and continues silently instead of failing to launch.
- Music was converted from MP3 to OGG to avoid depending on an external MP3 decoder;
  `libogg-0.dll` ships next to the executable.

### Wrong-Orientation Hit Sound
- Added `assets/hit_wrong.wav`, played when a player bullet strikes an enemy whose
  orientation does not match the shot. Portrait enemies hit at 90° and landscape
  enemies hit at 0° both trigger it.
- The shot is still consumed on contact (bullets never pass through enemies), so the
  sound reinforces the rule that the enemy survived only because of the axis.
- Trigger lives in the `else` branch of `Enemy::IsVulnerableAt(bulletQt)` inside
  `Game::HandleCollisions()`, using the bullet's fire-time quarter turn rather than
  the current view angle.
- Played through `MIX_PlayAudio` and throttled by `kWrongHitSfxCooldown` (60 ms) so
  several bullets landing on the same frame do not stack into clipping.
- Loaded in `Game::LoadAssets()` and freed in `Game::UnloadAssets()`, guarded by
  `RTZ_USE_SDL3_MIXER` like the other audio.
- A placeholder sample was generated with the Python standard library (0.22 s,
  48 kHz, 16-bit mono) so the effect works out of the box; replace the file to change it.

### Project Rename
- Renamed the template project from `SDL TEST` to `ROTATORZ`: `ROTATORZ.slnx`,
  `ROTATORZ/ROTATORZ.vcxproj` and `ROTATORZ/ROTATORZ.cpp`.
- The output binary is now `x64\Debug\ROTATORZ.exe`, and `RootNamespace` is `ROTATORZ`.
- Updated the solution's project path, the `.vcxproj.filters` entry, `.gitignore`
  build/DLL paths, and the README repository tree.
- Renames were done with `git mv`, so file history follows the moved files.

### Enemy Shooting Sound
- Added `assets/enemy_shoot.wav`, played from `Game::PlayEnemyShootSound()` whenever a
  front-line enemy spawns a bullet.
- Because enemy fire is frequent, the sample plays at `kEnemyShootGain` (0.35) instead
  of full volume. Adjust that constant to taste.
- Playback uses four round-robin tracks (`kEnemyShootVoices`) created once in
  `LoadAssets()`, each with the gain preset, so simultaneous shots overlap instead of
  restarting one shared track and cutting each other off.
- Tracks and audio are freed in `UnloadAssets()` under the usual `RTZ_USE_SDL3_MIXER`
  guards; a missing file only logs a message and the game keeps running.


---