#pragma once

#include <SDL3/SDL.h>

namespace rtz {

// Abstract base class for everything that lives in the world.
// Subclasses override Update and Render; the game loop only talks to this
// interface, which keeps the update/render code free of type checks.
class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;
    virtual SDL_FRect GetBounds() const = 0;

    // Pool lifecycle: Activate() is called when object is acquired from pool,
    // Deactivate() when returned. Default implementations just flip the flag.
    virtual void Activate() { active = true; }
    virtual void Deactivate() { active = false; }

    // Variadic Activate for pools that pass constructor args.
    template <typename... Args>
    void Activate(Args&&...) { active = true; }

    bool IsActive() const { return active; }
    void Destroy() { active = false; }

protected:
    bool active = true;
};

} // namespace rtz
