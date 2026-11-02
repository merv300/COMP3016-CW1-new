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
| **Unique Mechanic** | Enemies are **portrait-only** or **landscape-only** killable. Press **Q** to rotate the entire world 90° and remap input accordingly. |
| **Controls** | See [Controls](#controls) |

---

## 🕹 Controls

| Input | Action |
|-------|--------|
| **A / D** or **← / →** | Strafe left / right |
| **W / S** or **↑ / ↓** | Move forward / back (relative to current rotation) |
| **Left Click** / **Space** | Fire |
| **Right Click** / **Shift** | Bomb / Special |
| **Q** | Rotate view 90° clockwise |
| **Esc** | Pause / Back to menu |

> Input vectors are automatically remapped when the view rotates so "up" always means "toward the top of the rotated screen".

---

## 🛠 Build & Run

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
| **Core Systems** | Player movement, input remapping, fixed timestep loop | 🔲 |
| **Rotation Mechanic** | Off-screen texture + `SDL_RenderTextureRotated`, input remap on rotate | 🔲 |
| **Enemy System** | Orientation tags (portrait/landscape), spawning, object pool | 🔲 |
| **Combat & Progression** | Collision, scoring, lives, wave/difficulty scaling | 🔲 |
| **State Machine** | Menu / Playing / Paused / GameOver / Leaderboard | 🔲 |
| **Persistence** | High scores (`scores.dat`), config file loading | 🔲 |
| **Polish** | Particles, screen shake, sound (irrKlang), UI/menus | 🔲 |
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
