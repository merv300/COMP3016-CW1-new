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
| **Controls** | See [Controls](#controls) |

---

## 🕹 Controls

| Input | Action |
|-------|--------|
| **A / D** or **← / →** | Strafe side-to-side along world X (always horizontal; the view rotation is visual) |
| **W / S** or **↑ / ↓** | *(not used — axis-locked side movement only)* |
| **Left Click** / **Space** | Fire |
| **Right Click** | Bomb — detonates on contact and kills all enemies in its blast radius (5 s cooldown) |
| **Q** | Toggle view 0° ↔ 90° CW (smooth tween) |
| **F11** | Toggle fullscreen (desktop resolution) |
| **Esc** | Quit / back |

### Scoring
- Game starts at the **Main Menu** (Start Game / Leaderboard / Exit)
- Score scales with the wave number (100 × wave per kill)
- Bomb kills award a flat score per enemy
- High scores are saved to `scores.dat` (top 10, name + score)
- Game Over menu: choose **Retry** (restart immediately) or **Enter Name** (save score to leaderboard). Typed characters render in the entry field with a blinking cursor
- Leaderboard displays top 10 scores. Press Space on the leaderboard to return to the menu

> The world is a fixed 480×800 playfield rendered off-screen, then scaled to fit the 1280×720 window and rotated about its centre. Input is never remapped: A/D always moves along world X.

---

## 🛠 Build & Run

### Running the game (no build required)

The submission package `ROTATORZ.zip` contains a ready-to-run build:

```
ROTATORZ/
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
    └── hit_wrong.wav
```

Extract the zip and double-click `ROTATORZ.exe`. Keep the `.dll` files and the `assets/`
folder in the same directory as the executable.

> **Important:** assets are loaded relative to the *working directory*. If you move the
> executable, move `assets/` with it, and start the game from that folder. The game also
> writes `scores.dat` into the working directory.

### Building from source

#### Prerequisites

| Requirement | Notes |
|-------------|-------|
| **Windows 10/11 x64** | The project targets `x64` only |
| **Visual Studio 2026 (v18)** with **Desktop development with C++** | The project uses the v18 toolset. VS 2022 can open the sources but the platform toolset must be retargeted |
| **SDL3** development package (VC, x64) | Headers + `SDL3.lib` + `SDL3.dll` |
| **SDL3_image 3.4.8** development package (VC, x64) | `SDL3_image-devel-3.4.8-VC.zip` |
| **SDL3_mixer 3.2.4** development package (VC, x64) | `SDL3_mixer-devel-3.2.4-VC.zip` |

The project file (`ROTATORZ/ROTATORZ.vcxproj`) references the SDL packages by absolute
path, for both **Debug|x64** and **Release|x64**:

```
C:\schoolwork\Computer Science\COMP3016\SDL\SDL3\
C:\schoolwork\Computer Science\COMP3016\SDL\SDL3_image\SDL3_image-3.4.8\
C:\schoolwork\Computer Science\COMP3016\SDL\SDL3_mixer\SDL3_mixer-3.2.4\
```

If your SDKs live somewhere else, edit the `AdditionalIncludeDirectories`,
`AdditionalLibraryDirectories` and `LibraryPath` entries in `ROTATORZ.vcxproj` (there are
matching blocks for each configuration).

#### Build steps

1. Open **`ROTATORZ.slnx`** in Visual Studio.
2. Select **Release** and **x64** in the toolbar.
3. **Build → Build Solution** (`Ctrl+Shift+B`). The executable is written to
   `x64\Release\ROTATORZ.exe`.
4. Copy the runtime libraries next to the executable:
   - `SDL3.dll` from `ROTATORZ\SDL3.dll`
   - `SDL3_image.dll` from `SDL3_image-3.4.8\lib\x64\`
   - `SDL3_mixer.dll` and `libogg-0.dll` from `SDL3_mixer-3.2.4\lib\x64\` (`libogg-0.dll`
     is in `lib\x64\optional\`)
   - Optional image-format decoders from `SDL3_image-3.4.8\lib\x64\optional\`
     (`libpng16-16.dll`, `libwebp-7.dll`, …)
5. Copy the `assets\` folder next to `ROTATORZ.exe`.
6. Run `x64\Release\ROTATORZ.exe` **from that folder**.

Or build from a terminal in the repository root (adjust the MSBuild path to your install):

```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" `
  "ROTATORZ.slnx" /p:Configuration=Release /p:Platform=x64
```

For a debug build, use `Configuration=Debug`; the output goes to `x64\Debug\` and needs
the same DLLs and `assets\` folder.

#### Build notes

- `RTZ_USE_SDL3_IMAGE` and `RTZ_USE_SDL3_MIXER` are defined per configuration in the
  project. Without them the game still compiles, but sprites fall back to coloured
  rectangles and all audio is silent.
- Running from Visual Studio uses the project folder as the working directory, so the
  game will not find `assets/` there. Either run the built executable directly or set
  the debugger working directory to `$(OutDir)`.
- Missing sprite files are non-fatal (rectangles are drawn instead); missing audio files
  are non-fatal and only logged, as is an unavailable audio device.

### Dependencies

| Library | Purpose | Source |
|---------|---------|--------|
| **SDL3** | Window, rendering, input, audio device | [github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL) |
| **SDL3_image** | Loading PNG/JPEG sprites | [github.com/libsdl-org/SDL_image](https://github.com/libsdl-org/SDL_image) |
| **SDL3_mixer** | Looping music and sound effects | [github.com/libsdl-org/SDL_mixer](https://github.com/libsdl-org/SDL_mixer) |

### What Changed (Movement Reimplementation)

| Aspect | Before | After |
|--------|--------|-------|
| Player movement | Vec2 with `WorldFromScreen` remapping | Float X only, no remapping |
| Rotation | Instant snap via `quarterTurns` int | Smooth tween via `currentAngle`/`targetAngle` |
| Player Y | Axis-locked (bottom at 0°, left at 90°) | Always locked near the bottom edge |
| Bullet direction | Rotated with view | Always world UP (negative Y) |
| Enemy spawn | Random top-edge spawns | 6×4 grid waves, rows alternate orientation |
| Enemy fire | None | Front-line enemies shoot down at the player |
| Bullet collision | Passed through wrong-coloured enemies | Consumed on any enemy contact; only matching orientation destroys |
| Bomb | Planned only | Right-click bomb with 5 s cooldown and orientation-ignoring AoE |
| High scores | None | Top-10 table saved to scores.dat; name entry at Game Over; leaderboard state |
| World / window | World matched the 1280×720 window | 480×800 world scaled and centred inside the window |
| Vulnerability | Backwards (fixed) | LANDSCAPE at 90°, PORTRAIT at 0° |

---

## 📁 Repository Structure

```
ROTATORZ/
├── ROTATORZ.slnx
├── ROTATORZ/                    # C++ source project
│   ├── ROTATORZ.cpp
│   ├── ROTATORZ.vcxproj
│   ├── ROTATORZ.vcxproj.filters
│   ├── Game.cpp / Game.h
│   ├── Player.cpp / Player.h
│   ├── Enemy.cpp / Enemy.h
│   ├── Bullet.cpp / Bullet.h
│   ├── Bomb.cpp / Bomb.h
│   ├── Explosion.cpp / Explosion.h
│   ├── Star.cpp / Star.h
│   ├── InputManager.cpp / InputManager.h
│   ├── ObjectPool.h
│   ├── GameObject.h
│   ├── Vec2.h
│   ├── Constants.h
│   └── SDL3.dll
├── x64/
│   ├── Debug/                   # debug build output
│   └── Release/                 # release build: ROTATORZ.exe, DLLs, assets/
├── dist/                        # submission package (ROTATORZ.zip)
├── README.md
├── project.md
└── ai-development-log.md
```

### Built game layout

```
x64/Release/
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
    └── hit_wrong.wav
```

---

## 📄 Project Report

See **[`project.md`](./project.md)** for the complete write-up covering all 9 required sections (proposal deviations, gameplay, dependencies, patterns, mechanics, UML, testing, playtesting, evaluation).

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
| **Rotation Mechanic** | Off-screen world texture + `SDL_RenderTextureRotated` (rotation is visual; input stays axis-locked) | ✅ **Done** |
| **Enemy System** | Orientation tags (portrait/landscape), 6×4 wave grid, object pools | ✅ **Done** |
| **Combat & Progression** | Collision, orientation-gated kills, scoring, lives, wave scaling, bomb | ✅ **Done** |
| **State Machine** | Menu / Playing / GameOver / Leaderboard | ✅ **Done** |
| **Persistence** | Top-10 high scores saved to `scores.dat` | ✅ **Done** |
| **Polish** | Starfield, explosions, hit flash, SDL3_mixer music & SFX, menus | ✅ **Done** |
| **Playtesting** | Tester sessions, balancing, evidence in `playtesting/` | 🔲 **Not started** |
| **Finalisation** | Report write-up, submission zip (`dist/ROTATORZ.zip`), GitHub polish | 🟡 **In progress** |
| **Demo video** | Record and link a ≤10 min walkthrough | 🔲 **Not started** |

---

## 📺 Demo Video

- **YouTube (unlisted, 1080p, ≤10 min)**: *[link to be added]*
- **DLE submission**: *[filename to be added]*

---

## 📦 Submission Checklist

All 15+ items from the brief covered (repo, video, proposal, gameplay, dependencies, AI log, patterns, mechanics, UML, screenshots, test cases, playtesting, evaluation, compilable source, standalone exe + assets zip ≤150 MB, AI declaration).

---
