#pragma once

#include <SDL3/SDL.h>
#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

// Parallax star that drifts downward and wraps around the world.
// Purely cosmetic background for the starfield.
class Star final : public GameObject {
public:
    // Activate arguments: x, y, size, speed, alpha
    void Activate(float x, float y, float sizeValue, float speedValue,
                  Uint8 alphaValue);

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

private:
    Vec2 position;
    float size = 1.0f;
    float speed = 0.0f;
    Uint8 alpha = 255;
};

} // namespace rtz
