# ROTATORZ

**COMP3016 Immersive Game Technologies — Coursework 1**  
A 2D space shooter in C++ / SDL3 where the screen rotates 90° to expose orientation-locked enemies.

> **Survive endless waves of orientation-locked enemies by rotating the screen 90° to match their vulnerability.**

---

## 🎮 Gameplay Overview

| Aspect | Details |
|--------|---------|
| **Genre** | Arcade / Twin-stick shooter / Endless survival |
| **Core Loop** | Move → Shoot → Rotate view (Q) to match enemy orientation → Score → Survive |
| **Unique Mechanic** | Enemies are **portrait-only** or **landscape-only** killable. Press **Q** to rotate the view 90° so the matching enemies become vulnerable. |

---

## 🕹 Controls

| Input | Action |
|-------|--------|
| **A / D** or **← / →** | Strafe side-to-side along world X (always horizontal; view rotation is visual) |
| **Left Click** / **Space** | Fire |
| **Right Click** / **E** | Bomb — detonates on contact and kills all enemies in its blast radius (5 s cooldown) |
| **Q** | Toggle view 0° ↔ 90° CW (smooth tween) |
| **F11** | Toggle fullscreen |
| **Esc** | Quit / back |

### Scoring
- Game starts at the **Main Menu** (Start Game / Leaderboard / Exit)
- Score scales with wave number (100 × wave per kill)
- Bomb kills award a flat score per enemy
- High scores saved to `scores.dat` (top 10, name + score)
- Game Over menu: **Retry**, **Enter Name**, or **Main Menu**. Navigate with **Up/Down** or **W/S**, select with **Enter**
- Leaderboard displays top 10 scores. Press **Space** to return to menu
- **Survival timer**: counts time alive, shown top-right (M:SS, below score)

> The world is a fixed 480×800 playfield rendered off-screen, then scaled and rotated to fit the window. Input is never remapped: A/D always moves along world X.

---

## 🛠 Build & Run (Read This First — Examiners Start Here)

### Option 1: Run without building (quickest)

1. Download `dist/ROTATORZ.zip` from the GitHub Releases page.
2. Extract it anywhere (e.g. your Desktop).
3. **Keep everything together** — do not separate `ROTATORZ.exe` from the `.dll` files or the `assets/` folder.
4. Double-click `ROTATORZ.exe` to play.

---

### Option 2: Build from source (Visual Studio)

You need **one thing installed**: **Visual Studio 2022 or 2026** with the **"Desktop development with C++"** workload.

You do **not** need to download SDL yourself — the SDL libraries are already bundled in this repo under `Dependencies/`.

#### Step 1: Open the solution

1. Open File Explorer and go to this repo folder.
2. Double-click **`ROTATORZ.slnx`**. It will open in Visual Studio.

#### Step 2: Choose the build configuration

In the top toolbar of Visual Studio, set:
- **Configuration:** `Release` (or `Debug`)
- **Platform:** `x64`

> ⚠️ **Do not use `x86` or `Win32`.** The project is x64 only.

#### Step 3: Build

Press **`Ctrl + Shift + B`** (Build → Build Solution).

Wait for the Output window to say `Build: 1 succeeded, 0 failed`.

#### Step 4: Find your exe

After building, the executable is here:

```
x64\Release\ROTATORZ.exe      (Release build)
x64\Debug\ROTATORZ.exe        (Debug build)
```

#### Step 5: Run it

**Important:** Do **not** press the green "Play" / "Start" button inside Visual Studio. Visual Studio runs the exe from the project folder, where the `assets/` folder is not located, so the game will not find its images/sounds and will fail to start.

Instead:
1. Open File Explorer.
2. Go to `x64\Release\` (or `x64\Debug\`).
3. Double-click **`ROTATORZ.exe`**.

The DLLs are automatically copied next to the exe by the build (see post-build step below), and the `assets/` folder is already there.

---

### What happens automatically when you build

The project file is configured so that every successful build copies these DLLs into the output folder next to `ROTATORZ.exe`:

- `SDL3.dll`
- `SDL3_image.dll`
- `SDL3_mixer.dll`
- `libogg-0.dll`
- `libpng16-16.dll`
- `libwebp-7.dll`
- `libwebpdemux-2.dll`
- `libwebpmux-3.dll`

You do **not** need to copy anything manually.

---

### Repository structure (what goes where)

```
COMP3016-CW1-/
├── ROTATORZ.slnx                 # Visual Studio solution (open this)
├── README.md                     # this file
├── report.md                     # coursework report
├── ai-development-log.md         # development diary
├── Dependencies/                 # SDL libraries (bundled — do not delete)
│   ├── SDL3/
│   │   ├── include\SDL3\
│   │   └── lib\x64\SDL3.dll, SDL3.lib
│   ├── SDL3_image\SDL3_image-3.4.8/
│   │   ├── include\SDL3_image\
│   │   └── lib\x64\SDL3_image.dll + optional\ decoders
│   └── SDL3_mixer\SDL3_mixer-3.2.4/
│       ├── include\SDL3_mixer\
│       └── lib\x64\SDL3_mixer.dll + optional\libogg-0.dll
├── ROTATORZ/                     # C++ source project
│   ├── ROTATORZ.cpp              # entry point (main)
│   ├── ROTATORZ.vcxproj          # project file (build settings)
│   ├── Game.cpp / Game.h         # game loop, state machine, collisions, audio, UI
│   ├── Player.cpp / Player.h     # player ship
│   ├── Enemy.cpp / Enemy.h       # orientation-locked enemies
│   ├── Bullet.cpp / Bullet.h     # player and enemy bullets
│   ├── Bomb.cpp / Bomb.h         # player bomb
│   ├── Explosion.cpp / Explosion.h
│   ├── Star.cpp / Star.h         # background stars
│   ├── Planet.cpp / Planet.h     # scrolling background planets
│   ├── InputManager.cpp / InputManager.h
│   ├── ObjectPool.h              # object pooling
│   ├── GameObject.h              # base class for all entities
│   ├── Vec2.h                    # 2D vector maths
│   └── Constants.h               # gameplay tuning values
├── x64/
│   ├── Debug/                    # debug build output
│   └── Release/                  # release build output (exe + dlls)
├── dist/                         # submission package (ROTATORZ.zip)
└── assets/                       # game art and audio (root copy)
```

---

### Troubleshooting

| Problem | Cause | Fix |
|---------|-------|-----|
| `SDL3.dll not found` | Running from inside Visual Studio | Run `x64\Release\ROTATORZ.exe` from File Explorer instead |
| `assets/` not found | Wrong working directory | Run the exe directly from its folder, not from VS |
| Build fails with SDL errors | Wrong platform | Set platform to `x64`, not `x86` or `Win32` |
| Build fails, missing SDL headers | `Dependencies/` missing or moved | Make sure `Dependencies/` sits next to `ROTATORZ.slnx` |
| Game launches but no sound | Audio device or file missing | Non-fatal — game still runs, just silently |
| Sprites show as coloured rectangles | `RTZ_USE_SDL3_IMAGE` not defined | Should be automatic; check `ROTATORZ.vcxproj` defines |

---

## 📁 Repository Structure

(See the tree above in the Build section.)

### Built game output layout

After a Release build, `x64\Release\` contains:

```
x64\Release\
├── ROTATORZ.exe
├── SDL3.dll
├── SDL3_image.dll
├── SDL3_mixer.dll
├── libogg-0.dll
├── libpng16-16.dll
├── libwebp-7.dll
├── libwebpdemux-2.dll
├── libwebpmux-3.dll
└── assets/
    ├── player.png
    ├── enemy_portrait.png
    ├── enemy_landscape.png
    ├── music.ogg
    ├── shoot.wav
    ├── enemy_shoot.wav
    ├── hit_wrong.wav
    ├── bomb.wav
    ├── planet1.png
    ├── planet2.png
    └── planet3.png
```

---

## 📄 Project Report

See **[`report.md`](./report.md)** for the complete write-up covering all 9 required sections (proposal deviations, gameplay, dependencies, patterns, mechanics, UML, testing, playtesting, evaluation).

---

## 🤖 AI Development Log

See **[`ai-development-log.md`](./ai-development-log.md)** for transparency.

| Category | Tools Used |
|----------|------------|
| Code generation / debugging / refactoring | **OpenCode (Nemotron 3 Ultra), DeepSeek** |
| Asset creation | — (planned: Stable Diffusion / Firefly) |
| Audio | — (planned: Suno) |
| Documentation | OpenCode |

---

## 🎯 Development Milestones

| Phase | Milestone | Status |
|-------|-----------|--------|
| **Setup** | Project setup, Git workflow, window/renderer | ✅ **Done** |
| **Core Systems** | Player movement, delta-time loop with 50 ms clamping | ✅ **Done** |
| **Rotation Mechanic** | Off-screen world texture + `SDL_RenderTextureRotated` | ✅ **Done** |
| **Enemy System** | Orientation tags, 6×4 wave grid, object pools | ✅ **Done** |
| **Combat & Progression** | Collision, orientation-gated kills, scoring, lives, bomb | ✅ **Done** |
| **State Machine** | Menu / Playing / GameOver / Leaderboard | ✅ **Done** |
| **Persistence** | Top-10 high scores saved to `scores.dat` | ✅ **Done** |
| **Polish** | Starfield, explosions, hit flash, SDL3_mixer music & SFX, menus | ✅ **Done** |
| **Finalisation** | Report write-up, submission zip, GitHub polish | 🟡 **In progress** |

---

## 📺 Demo Video

- **YouTube (unlisted, 1080p, ≤10 min)**: *[https://youtu.be/5KKdsaFVGT0]*

---
