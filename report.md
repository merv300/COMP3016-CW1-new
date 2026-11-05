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

**Initial Proposal** 
ROTATORZ (CW1 Proposal)
Space invaders shooter-like game, you’re piloting a gunship in space, and you kill enemies using
projectile bullets and can kill them in either 1 or 2 shots, ideally not too difficult. The primary
mechanic will be the rotation of the actual game screen itself to kill enemies that can’t be killed
until the screen is rotated. Essentially, the closest wave of enemies at a given time would be
killed in a vertical screen rotation, but then a different coloured enemy could be on the back line
and can only be killed from when the screen is rotated and when the player is shooting left to
right.
Gameplay Loop
There is one infinite level; the goal is to survive as long as possible since you have 3 lives and
you have one HP. The player would have to manage all types of enemies and can rotate the
screen at any time. A timer and a score meter should be at the top of the screen. The longer you
survive the higher the score will be. Aim to get the highest score and could be a leaderboard
system which records against some predefined scores/your own personal best score.

There should be an item that’s on cooldown that can kill both landscape/portrait specific
enemies. Universal bomb item will spawn between the enemies in the row. Can be shot and
then recharges the item. Could be a bomb the player shoots and explodes (between the
enemies in the row) or a missile shot from the player character. There will be an animated
background in space with some stars and passing planets to have there be some visual
feedback. Enemy AI shouldn’t overwhelm the player too much and the bullets/projectiles
should be manageable. The colour of the enemies should indicate that they are
landscape/portrait killable enemies.
Vertical orientation ex. Horizontal orientation ex.
A and D should be used for movement keys, will work as left/right then change to up/down upon
orientation change. Left click for the normal gun pellets, right click for potential bomb item. Q
key should be the screen orientation change key.

White player character on black space backdrop. Orange can be portrait killable enemies; green
enemies can be killed in landscape. The screen will rotate 90 degrees clockwise, then rotate
back counterclockwise to the other orientation. The movement should stay the same from going
along the x-axis to the y-axis for movement, the movement won’t factor in forward and
backward movement. The user interface would need to be rotated alongside everything else and
this rotation would take place inside a regular 16:9 game window. Essentially a portrait->
landscape rotation. I’d ideally need to change components between the x and the y axis but
need to figure out how to calculate the positions as the orientation changes.
There will be a starting screen which you can start the game from with a little explanation as to
how the game mechanics will work. There will be a leaderboard to track the scores and a button
to return to the menu screen from here. You should also be able to exit the game from here.
There should be sufficient sound effects for the bullets and the enemies being killed; alongside
some game music that is able to be looped while the player continues a run. The visuals should
provide enough information to the player as to if they’ve killed an enemy and for when they’ve
been killed (game over screen).
The player and enemies have a given amount of health. The player character will be killed after
being hit with one bullet but will have 3 lives total. The player will respawn in the same position
and will retain the current time and score. Once the lives have depleted, it’ll be game over and
the score will be recorded on the leaderboard if it’s a high score.
Gameplay screen mock-up
AI Usage
AI will be used in an assistive sense in order to generate potential improvements to this
proposed game loop and for figuring out how to scale the game with the screen rotation. AI
should be used to troubleshoot potential build errors and for bug fixes too. AI Code generation
will be monitored and will help the game’s optimisation and its feature set. I intend on using
OpenCode to refine the codebase for the project and will document its contributions and how
the model has refined the codebase/added to it.
Development

I will start by familiarising myself with SDL3 graphics programming and its functions in order to
import some simple sprites and how to put some background planets within a scrolling
animated background. I can prototype the initial shooting mechanics first with the base game’s
loop and then figure out how the implementation of the screen rotation mechanic can fit with
the initially programmed game loop.
I will be using OOP Design Paradigms to structure the project and utilise an Entity Component
System to structure the program. The enemies should all be grouped under a general class and
the player character’s health and powerup/item amount will be attributes separate to this. The
score and the timer should be tracked per playthrough and will be saved inbetween runs for the
player to view in the leaderboard section.
I plan on playtesting this to balance the mechanics and aspects such as the enemy respawn
frequency, life amount and fire rate of the character. These aspects can be improved with
extensive playtesting and iteration on the balancing. I will show the work in progress projects to
my friends and document their feedback via bug reports and surveys: these forms should be
frequently made and ask meaningful questions about the current game state.

---

## 2. Gameplay

[Short paragraph: loop, scoring, lives, difficulty ramp. Describe the moment-to-moment experience, how rotation changes strategy, risk/reward of rotating vs. repositioning.]

The gameplay loop will include shooting alien enemies in order to get your score as high as possible! There'll be a risk and reward from the rotation of the screen for when you won't be able to destroy the other type of enemy, which creates urgency for the player.  The strategy inverts the typical Space Invaders game loop. 
---

## 3. Dependencies

- **SDL3** — window, renderer, input, audio
- SDL3_image — texture loading (PNG)
- SDL3_mixer — sound effects & music (looping background track, shoot SFX, wrong-orientation hit SFX)

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
- **Player movement:** horizontal-only in world X (A/D), Y locked near the bottom edge. No input remapping — rotation is purely visual.
- **World / window:** a fixed 480×800 world is rendered off-screen and scaled to fit the 1280×720 window, preserving aspect ratio.
- **Display:** `F11` toggles borderless fullscreen via `SDL_SetWindowFullscreen`; the renderer queries the live window pixel size each frame so the rotated world stays centred at any resolution.
- **Starfield:** 160 parallax stars in 3 layers drift downward and wrap around the world, rendered behind the grid and entities.
- **Rotation:** `currentAngle`/`targetAngle` tween pair; Q toggles between 0° and 90°
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
- View rotates smoothly between portrait (0°) and landscape (90° CW) via `Q` toggle
- `currentAngle` tweens toward `targetAngle` at `kRotationSpeed` (300°/s)
- Rotation is **purely visual**: the world does not actually rotate; player movement,
  bullet trajectories, and collision all stay in world space
- `SDL_RenderTextureRotated` presents the off-screen render texture at `currentAngle`
- The rotated world's axis-aligned bounding box determines the scale, so the whole playfield stays visible at either orientation:
  ```cpp
  float aabbW = kWorldWidth * |cos| + kWorldHeight * |sin|;
  float aabbH = kWorldWidth * |sin| + kWorldHeight * |cos|;
  float scale = min(kWindowWidth / aabbW, kWindowHeight / aabbH);
  ```
- The scaled destination rect is centred in the window and rotated about its centre

### Enemies
- Each enemy carries `EnemyOrientation`: `PORTRAIT` (orange) or `LANDSCAPE` (green)
- Waves spawn as a 6×4 grid; rows alternate orientation so the player must flip the view
- The formation descends steadily; the lowest enemy in each column is the "front line" and fires downward
- A bullet is consumed on contact with **any** enemy, but only destroys it when the current view rotation matches the enemy's orientation — wrong-coloured enemies block shots rather than letting them pass through
- Colour is the visual indicator of the required orientation (orange = portrait, green = landscape)

### Bomb & Explosions
- Right-click launches a bomb upward at 380 px/s; it is gated by a 5-second cooldown that starts fully charged
- The bomb detonates on contact with **any** enemy, ignoring orientation
- Every enemy within the 80 px blast radius dies and awards a flat bomb score, making it a panic button for mixed-colour waves
- The explosion is a short-lived visual effect: a growing outer ring plus a shrinking hot core that fades over 0.4 s

### Scoring / Lives / Respawn
- Base score per kill × wave multiplier
- 3 lives; respawn grants 2s invincibility (blinking sprite)
- Game starts at **Main Menu** (Start Game / Leaderboard / Exit)
- Game Over → menu: **Retry** (restart immediately) or **Enter Name** (save score to leaderboard)
- **Name entry:** typed characters render in a bordered field with a blinking cursor; Enter saves and opens Leaderboard
- Game Over → enter name → Leaderboard
### File Loading
- `scores.dat` — binary high score table (top 10), loaded at startup and saved when a new score qualifies.
- `assets/config.json` — keybinds, volume, difficulty preset (planned).

### Static Analysis (C6385/C6386)
- All high score table accesses in `AddHighScore()`, `LoadHighScores()`, `SaveHighScores()`, and `RenderLeaderboard()` use explicit bounds checks against `kMaxHighScores` and `highScoreCount` to suppress Microsoft Code Analysis warnings C6385 (invalid data access) and C6386 (buffer overrun) without `#pragma warning(disable)`.
- `AddHighScore()` re-validates `insertAt` immediately before the final write because the analyzer cannot track the variable across the shift loop.

---

## 6. Initial UML
uml.png

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

**Participants:** [Friend 1, Friend 2]  
**Evidence:** `playtesting/` 

### Changes Made in Response

| Feedback | Change | Commit |
|----------|--------|--------|
| [e.g. "text"] | [make the text yellow, there's a 0 and 1 on the game over screen
selected text should appear yellow, the text in general should be white.]

**Reflection:** [It was good giving a build to two of my friends and asking about various quality of life features. They liked the game loop and only really found difficulty with reading some of the text so I was able to change this to work better and to scale better with the display.]

---

## 9. Evaluation

### What Went Well
- [Rotation mechanic feels unique and polished]
- [Integration of sdl_image and sdl_mixer]


### What I'd Do Differently
- [Integrate SDL3_ttf earlier for asset pipeline]
- [More frequent playtesting earlier in development]
- [Add more visual flair for the project]

### Proud of 
- [Gameplay loop is satisfying]
- [Handmade audio samples for game sound suitable]
- [Game balancing with bullet rate of fire and the bomb cooldown]


