#pragma once

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

class Player final : public GameObject {
public:
    Player(Vec2 startPosition, float size);

    // Intent is supplied in world space (already rotation-corrected by Game),
    // which keeps the player itself unaware of view rotation.
    void SetMoveIntent(const Vec2& worldDirection);
    void SetFiring(bool value) { firing = value; }

    // Records the current view rotation so the ship is drawn facing screen-up.
    void SetFacingQuarterTurns(int turns) { facingQuarterTurns = ((turns % 4) + 4) % 4; }

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    Vec2 GetPosition() const { return position; }

private:
    Vec2 position;
    Vec2 moveDirection{0.0f, 0.0f};
    float size = 36.0f;
    float speed = 320.0f;
    bool firing = false;
    int facingQuarterTurns = 0;
};

} // namespace rtz
