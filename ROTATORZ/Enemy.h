#pragma once

#include <SDL3/SDL.h>
#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

// Which view rotation makes this enemy vulnerable.
// The game only supports two view states, but the enum keeps the intent
// explicit and lets the vulnerability test accept a quarter-turn index.
enum class EnemyOrientation {
    LANDSCAPE = 0, // green, vulnerable when the view is at 90°
    PORTRAIT  = 1  // orange, vulnerable when the view is at 0°
};

class Enemy final : public GameObject {
public:
    // Activate arguments:
    // startPos    - world-space centre of the enemy
    // orientation - which view rotation kills it (also drives its colour)
    // wave        - current wave number (scales score)
    void Activate(const Vec2& startPos, EnemyOrientation orientation, int wave);

    // Loads the two enemy sprites once (call from Game::Initialize). Falls back
    // to coloured rectangles if loading fails or this isn't called.
    static bool LoadTextures(SDL_Renderer* renderer,
                             const char* portraitPath,
                             const char* landscapePath);
    static void UnloadTextures();

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    EnemyOrientation GetOrientation() const { return orientation; }
    bool IsVulnerableAt(int quarterTurns) const;

    int GetScoreValue() const { return scoreValue; }

    Vec2 GetPosition() const { return position; }
    void SetPosition(const Vec2& pos) { position = pos; }

    // Front-line shooting timer. Game ticks it and spawns the bullet.
    float GetShootTimer() const { return shootTimer; }
    void SetShootTimer(float value) { shootTimer = value; }

private:
    Vec2 position;
    EnemyOrientation orientation = EnemyOrientation::LANDSCAPE;
    float hitFlashTimer = 0.0f;
    float shootTimer = 0.0f;
    float descendSpeed = 0.0f;
    int scoreValue = 100;
    int wave = 1;
    static constexpr float kHitFlashDuration = 0.1f;

    static SDL_Texture* s_portraitTexture;
    static SDL_Texture* s_landscapeTexture;
};

} // namespace rtz
