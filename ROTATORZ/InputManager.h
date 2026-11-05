#pragma once

#include <SDL3/SDL.h>
#include <unordered_set>

#include "Vec2.h"

namespace rtz {

// Owns all keyboard and mouse state. The game loop polls SDL events once per
// frame and forwards them here, so gameplay code never touches SDL_Event
// directly and can be driven by synthetic input when testing.
class InputManager {
public:
    // Clears the "pressed this frame" set. Call before polling events.
    void BeginFrame();

    // Handles a single SDL event. Call once per event returned by PollEvent.
    void ProcessEvent(const SDL_Event& event);

    // Refreshes the keyboard snapshot. Call after all events are processed.
    void EndFrame();

    bool IsKeyDown(SDL_Scancode scancode) const;
    bool WasKeyPressed(SDL_Scancode scancode) const;

    bool IsFiring() const { return firing; }
    bool IsRightMouseDown() const { return mouseRightDown; }
    bool QuitRequested() const { return quitRequested; }

    // Movement in screen space (normalised), built from WASD or the arrow keys.
    Vec2 GetMoveAxis() const;

private:
    const bool* keys = nullptr;
    std::unordered_set<int> pressedThisFrame;
    bool mouseLeftDown = false;
    bool mouseRightDown = false;
    bool firing = false;
    bool quitRequested = false;
};

} // namespace rtz
