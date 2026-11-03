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
| **Code Generation** | Boilerplate, system scaffolding, helper functions | Game loop template, StateMachine base class, ObjectPool<T>, RotationManager |
| **Debugging** | Error interpretation, crash analysis, logic bugs | SDL_Init failure handling, render texture setup, input remapping math, helped create gitignore file for initial documentation commit. |
| **Refactoring** | Modern C++ improvements, RAII, smart pointers, const-correctness | Converting raw pointers to `unique_ptr`, adding move semantics |
| **Documentation** | README, project.md structure, AI log, code comments | This file, README.md, project.md skeleton |
| **Architecture Discussion** | Pattern selection, class design, data flow | State vs. StateStack, ECS vs. composition, delta-time vs fixed timestep trade-offs |
---

## 3. Example Prompts



---

## 4. Verification Process

For each AI-generated output:

1. **Compile Test** — Paste into project, build in VS2022 (Release x64). Fix any compile errors.
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
- **Bugs introduced by AI** 
- **Performance choices** 
- **Creative decisions** (rotation mechanic, orientation-locked enemies, visual style) are entirely original.

---