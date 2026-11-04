#pragma once

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

class Player final : public GameObject {
public:
    Player(Vec2 startPosition, float size);

    // Intent is supplied as world-X only (-1 = left, 1 = right, 0 = none).
    // The view rotation is purely visual, so the player never remaps input.
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
