#pragma once

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

// Player-fired bomb that travels upward and detonates on contact with any
// enemy, damaging everything in its blast radius regardless of orientation.
class Bomb final : public GameObject {
public:
    // Activate arguments:
    // startPos - world position (just above the player)
    // velocity - world-space velocity (pixels/sec), normally straight up
    void Activate(const Vec2& startPos, const Vec2& velocity);

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    Vec2 GetPosition() const { return position; }

private:
    Vec2 position;
    Vec2 velocity;
    float life = 0.0f;
    static constexpr float kMaxLife = 4.0f;
};

} // namespace rtz
