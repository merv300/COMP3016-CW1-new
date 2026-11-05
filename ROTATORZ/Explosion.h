#pragma once

#include "Constants.h"
#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

// Short-lived visual effect: a growing outer ring and shrinking hot core
// that fades out. Purely cosmetic — it does not collide with anything.
class Explosion final : public GameObject {
public:
    // Activate arguments:
    // center - world-space centre of the blast
    void Activate(const Vec2& center);

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    const Vec2& GetCenter() const { return center; }
    float GetTimer() const { return timer; }

private:
    Vec2 center;
    float timer = 0.0f;
    static constexpr float kDuration = kExplosionDuration;
};

} // namespace rtz
