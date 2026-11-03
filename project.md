# ROTATORZ — Project Report

## 1. Proposal Summary & Deviations

**Game Title:** ROTATORZ  
**Genre:** 2D Arcade Space Shooter / Endless Survival  
**Synopsis:** Survive endless waves of orientation-locked enemies by rotating the screen 90° to match their vulnerability.

**Core Gameplay Loop:**
1. Enemies spawn in waves — each tagged as **Portrait** or **Landscape** killable
2. Player moves, shoots, and survives
3. When facing wrong-orientation enemies, press **Q** to rotate view 90°
4. Input remaps automatically; now you can hit the previously invulnerable enemies
5. Score increases, difficulty ramps, repeat

**Intended Mechanics:**
- Screen rotation (90° increments, clockwise only)
- Orientation-locked enemy vulnerability
- Twin-stick style movement + shooting
- Bomb/special ability
- Lives + respawning with brief invincibility
- Wave-based difficulty scaling

**Technical Features:**
- Delta-time game loop with clamped frame delta (50 ms cap)
- State machine (Menu/Playing/Paused/GameOver/Leaderboard)
- Object pooling for bullets/enemies
- Off-screen render texture rotated via `SDL_RenderTextureRotated`
- Input remapping via rotation-aware vector transform
- File I/O for high scores (`scores.dat`) and config

**External Libraries:**
- SDL3 (core)
- *Planned:* SDL3_image, SDL3_ttf, irrKlang (if time permits)

---

### Deviations from Initial Proposal

| Proposed | Delivered | Justification |
|----------|-----------|---------------|
| Fixed timestep game loop with accumulator | Delta-time loop with 50 ms clamp | Compressed 4-day schedule; fixed timestep adds ~40 lines + interpolation complexity with no benefit for an arcade shooter. Clamping prevents tunnelling after stalls. |
| [Feature] | [Status] | [Reason] |
| *Fill in as development progresses* | | |

---

## 2. Gameplay

[Short paragraph: loop, scoring, lives, difficulty ramp. Describe the moment-to-moment experience, how rotation changes strategy, risk/reward of rotating vs. repositioning.]

The gameplay loop will include shooting alien enemies in order to get your score as high as possible! There'll be a risk and reward from the rotation of the screen for when you won't be able to destroy the other type of enemy, which creates urgency for the player.  The strategy inverts the typical Space Invaders game loop. 
---

## 3. Dependencies

- **SDL3** — window, renderer, input, audio
- *(Optional/Planned)* SDL3_image — texture loading (PNG)
- *(Optional/Planned)* SDL3_ttf — font rendering for UI/scores
- *(Optional/Planned)* irrKlang — sound effects & music

> Currently only SDL3 is integrated to minimise build friction.

---

## 4. Patterns Used

- **State** — `GameState` machine (Menu / Playing / Paused / GameOver / Leaderboard)
- **Game Loop** — delta-time with clamped frame delta (50 ms cap)
- **Object Pool** — bullets and enemies
- **OOP / Inheritance** — `GameObject` base class with virtual `Update`/`Render`, derived `Player`, `Enemy`, `Bullet`, `Particle`
- [Add/remove to match what you actually built.]

---

## 5. Mechanics & Code

### Architecture
- Core systems (`Game`, `StateMachine`, `Renderer`, `InputManager`, `AssetManager`, `ObjectPool`) owned by `Game`
- **Game loop:** delta-time with `SDL_GetPerformanceCounter`, clamped to 50 ms (20 FPS floor) to prevent tunnelling after stalls
- All game entities inherit from abstract `GameObject`:
  ```cpp
  class GameObject {
  public:
      virtual ~GameObject() = default;
      virtual void Update(float dt) = 0;
      virtual void Render(SDL_Renderer* renderer) = 0;
      virtual SDL_FRect GetBounds() const = 0;
      bool IsActive() const { return active; }
      void Destroy() { active = false; }
  protected:
      bool active = true;
  };
  ```
- `Player`, `Enemy`, `Bullet`, `Particle` override `Update`/`Render` with type-specific logic
- `ObjectPool<GameObject*>` stores base pointers; type-specific pools (`ObjectPool<Bullet>`, `ObjectPool<Enemy>`) for cache-friendly allocation

### Rotation System
- World rendered to an off-screen `SDL_Texture` (target texture)
- Presented via `SDL_RenderTextureRotated(renderer, texture, NULL, NULL, angle, NULL, SDL_FLIP_NONE)`
- Angle cycles 0° → 90° → 180° → 270° → 0° on each Q press
- Input vectors transformed by rotation matrix so "up" always maps to screen-up

### Enemies
- Each enemy carries `OrientationRequired` enum: `PORTRAIT` or `LANDSCAPE`
- Bullet collision only registers damage when `currentViewOrientation == enemy.orientationRequired`
- Visual indicator (color/icon) shows required orientation

### Scoring / Lives / Respawn
- Base score per kill × wave multiplier
- 3 lives; respawn grants 2s invincibility (blinking sprite)
- Game Over → enter name → Leaderboard

### File Loading
- `assets/scores.dat` — binary or JSON high score table (top 10)
- `assets/config.json` — keybinds, volume, difficulty preset

---

## 6. UML

`docs/uml.png` — Class diagram showing:
- `Game` (owns `StateMachine`, `Renderer`, `InputManager`, `AssetManager`, `ObjectPool`s)
- `State` hierarchy (`MenuState`, `PlayingState`, `PausedState`, `GameOverState`, `LeaderboardState`)
- `GameObject` hierarchy: abstract base with virtual `Update(float dt)`, `Render(SDL_Renderer*)`, `GetBounds()` → `Player`, `Enemy`, `Bullet`, `Particle`
- `ObjectPool<T>` template (specialised for `Bullet`, `Enemy`, `Particle`)
- `RotationManager` / `InputRemapper`

---

## 7. Exception Handling & Test Cases

| # | Test | Expected | Result |
|---|------|----------|--------|
| 1 | Missing `assets/` folder | Graceful error message, fallback defaults, no crash | |
| 2 | Corrupt `scores.dat` | Warning logged, empty leaderboard used | |
| 3 | Spam Q during rotation animation | Input ignored until rotation completes; no double-rotate | |
| 4 | Alt-F4 / window close mid-run | Clean shutdown: SDL_Quit, file handles closed | |
| 5 | Invalid config.json | Defaults applied, warning logged | |
| 6 | SDL_Init failure | Error to stderr, exit code 1 | |

---

## 8. Playtesting

**Participants:** [N testers — peers / family, anonymised T1..Tn]  
**Evidence:** `playtesting/` (screenshots, notes, survey responses)

### Changes Made in Response

| Feedback | Change | Commit |
|----------|--------|--------|
| [e.g. "Rotation feels disorienting"] | [e.g. Added 150ms rotation animation with screen shake] | [hash] |

**Reflection:** [Who participated, what was tested, what changed, remaining limitations.]

---

## 9. Evaluation

### What Went Well
- [e.g. Rotation mechanic feels unique and polished]


### What I'd Do Differently
- [e.g. Integrate SDL3_image/ttf earlier for asset pipeline]
- [e.g. More frequent playtesting earlier in development]

### Proud Of
