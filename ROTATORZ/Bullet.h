#pragma once

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

// Distinguishes the player's shots from enemy fire so collisions can be
// filtered without a separate Bullet class hierarchy.
enum class BulletType {
    Player,
    Enemy
};

class Bullet final : public GameObject {
public:
    // Activate is called by ObjectPool when acquiring. Arguments:
    // startPos          - world position
    // direction         - normalised world-space velocity direction
    // speed             - pixels per second
    // type              - which side fired it (drives colour and collision filter)
    // firedQuarterTurns - view rotation at the moment the shot was fired.
    //                     Bullets keep this for their lifetime so rotating the
    //                     view mid-flight does not change what they can kill.
    void Activate(const Vec2& startPos, const Vec2& direction, float speed,
                  BulletType type = BulletType::Player,
                  int firedQuarterTurns = 0);

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    BulletType GetType() const { return type; }

    // Quarter-turn index (0 = portrait, 1 = landscape) captured at fire time.
    int GetFiredQuarterTurns() const { return firedQuarterTurns; }

private:
    Vec2 position;
    Vec2 velocity;
    BulletType type = BulletType::Player;
    int firedQuarterTurns = 0;
    float life = 0.0f;
    static constexpr float kMaxLife = 4.0f;
    static constexpr float kRadius = 4.0f;
};

} // namespace rtz
