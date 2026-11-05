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


### Requirements
- **Visual Studio 2022** (Desktop development with C++ workload)
- **Windows 10/11 x64**
- **SDL3** development libraries (included in `SDL TEST/SDL3.dll`)

### Open & Build


### Dependencies
| Library | Purpose | Source |
|---------|---------|--------|
| **SDL3** | Window, rendering, input, audio | [github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL) |

---

## 📁 Repository Structure

```
ROTATORZ/
├── SDL TEST.slnx
├── SDL TEST/
│   ├── SDL TEST.cpp
│   ├── SDL TEST.vcxproj
│   ├── SDL3.dll
│   └── assets/
│       ├── player.png
│       ├── enemy_portrait.png
│       ├── enemy_landscape.png
│       ├── music.ogg
│       ├── shoot.wav
│       └── hit_wrong.wav
├── docs/
├── playtesting/
├── README.md
├── project.md
└── ai-development-log.md
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
| **Setup** | Project setup, Git workflow, basic window/renderer | 🔲 **In progress** |
| **Core Systems** | Player movement, input remapping, delta-time loop with clamping | 🔲 |
| **Rotation Mechanic** | Off-screen texture + `SDL_RenderTextureRotated`, input remap on rotate | 🔲 |
| **Enemy System** | Orientation tags (portrait/landscape), spawning, object pool | 🔲 |
| **Combat & Progression** | Collision, scoring, lives, wave/difficulty scaling | 🔲 |
| **State Machine** | Menu / Playing / Paused / GameOver / Leaderboard | 🔲 |
| **Persistence** | High scores (`scores.dat`), config file loading | 🔲 |
| **Polish** | Particles, screen shake, sound (SDL3_mixer), UI/menus | 🔲 |
| **Playtesting** | Internal testing, balancing, bug fixes | 🔲 |
| **Finalisation** | Video recording, report write-up, zip submission, GitHub polish | 🔲 |

---

## 📺 Demo Video

- **YouTube (unlisted, 1080p, ≤10 min)**: *[link to be added]*
- **DLE submission**: *[filename to be added]*

---

## 📦 Submission Checklist

All 15+ items from the brief covered (repo, video, proposal, gameplay, dependencies, AI log, patterns, mechanics, UML, screenshots, test cases, playtesting, evaluation, compilable source, standalone exe + assets zip ≤150 MB, AI declaration).

---
