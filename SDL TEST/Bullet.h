#pragma once

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

class Bullet final : public GameObject {
public:
    // Activate is called by ObjectPool when acquiring. Arguments:
    // startPos - world position
    // direction - normalised world-space velocity direction
    // speed - pixels per second
    void Activate(const Vec2& startPos, const Vec2& direction, float speed);

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

private:
    Vec2 position;
    Vec2 velocity;
    float life = 0.0f;
    static constexpr float kMaxLife = 2.5f;
    static constexpr float kRadius = 4.0f;
};

} // namespace rtz