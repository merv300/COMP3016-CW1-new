#pragma once

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

// Which view rotation makes this enemy vulnerable.
// 0° (0) and 180° (2) are LANDSCAPE (wide).
// 90° (1) and 270° (3) are PORTRAIT (tall).
enum class EnemyOrientation {
    LANDSCAPE = 0, // vulnerable at 0° and 180°
    PORTRAIT  = 1  // vulnerable at 90° and 270°
};

class Enemy final : public GameObject {
public:
    // Activate arguments:
    // startPos - world position (spawned at edge)
    // velocity - world-space velocity (pixels/sec)
    // orientation - which rotation kills it
    // wave - current wave number (scales health/speed)
    void Activate(const Vec2& startPos, const Vec2& velocity, EnemyOrientation orientation, int wave);

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    EnemyOrientation GetOrientation() const { return orientation; }
    bool IsVulnerableAt(int quarterTurns) const;

    int GetScoreValue() const { return scoreValue; }

private:
    Vec2 position;
    Vec2 velocity;
    EnemyOrientation orientation = EnemyOrientation::LANDSCAPE;
    float hitFlashTimer = 0.0f;
    int scoreValue = 100;
    int wave = 1;
    static constexpr float kHitFlashDuration = 0.1f;
};

} // namespace rtz