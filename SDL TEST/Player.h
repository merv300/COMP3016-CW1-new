#pragma once

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

class Player final : public GameObject {
public:
    Player(Vec2 startPosition, float size);

    // Intent is supplied in world space (already rotation-corrected by Game),
    // which keeps the player itself unaware of view rotation.
    void SetMoveIntent(float worldXDir);
    void SetFiring(bool value) { firing = value; }

    // Current view angle in degrees (0 = portrait, 90 = landscape). Used only
    // for rendering the ship's nose indicator.
    void SetViewAngle(float degrees) { viewAngle = degrees; }

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    Vec2 GetPosition() const { return position; }
    void SetPosition(const Vec2& pos) { position = pos; }

private:
    Vec2 position;
    float moveIntentX = 0.0f;
    float size = 36.0f;
    float speed = 320.0f;
    bool firing = false;
    float viewAngle = 0.0f;
};

} // namespace rtz
