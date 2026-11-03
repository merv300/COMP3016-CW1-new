#include "Enemy.h"

#include <algorithm>

namespace rtz {

void Enemy::Activate(const Vec2& startPos, const Vec2& vel, EnemyOrientation orient, int waveNumber) {
    GameObject::Activate();
    position = startPos;
    velocity = vel;
    orientation = orient;
    wave = waveNumber;
    hitFlashTimer = 0.0f;

    // Scale score with wave
    scoreValue = 100 * wave;
}

void Enemy::Deactivate() {
    GameObject::Deactivate();
}

bool Enemy::IsVulnerableAt(int quarterTurns) const {
    const int normalized = ((quarterTurns % 4) + 4) % 4;
    if (orientation == EnemyOrientation::LANDSCAPE) {
        // Green enemies: killable in landscape mode (90° / 270°)
        return normalized == 1 || normalized == 3;
    } else {
        // Orange enemies: killable in portrait mode (0° / 180°)
        return normalized == 0 || normalized == 2;
    }
}

void Enemy::Update(float deltaTime) {
    position += velocity * deltaTime;

    if (hitFlashTimer > 0.0f) {
        hitFlashTimer -= deltaTime;
    }

    // Despawn if off-screen with margin
    const float margin = 60.0f;
    if (position.x < -margin || position.x > 800.0f + margin ||
        position.y < -margin || position.y > 800.0f + margin) {
        Deactivate();
    }
}

void Enemy::Render(SDL_Renderer* renderer) {
    // Shape indicates required orientation:
    // LANDSCAPE = wide rectangle (horizontal)
    // PORTRAIT = tall rectangle (vertical)
    const float width  = (orientation == EnemyOrientation::LANDSCAPE) ? 56.0f : 28.0f;
    const float height = (orientation == EnemyOrientation::LANDSCAPE) ? 28.0f : 56.0f;

    const SDL_FRect rect{
        position.x - width * 0.5f,
        position.y - height * 0.5f,
        width,
        height
    };

    // Base color by orientation (proposal: orange = portrait, green = landscape)
    Uint8 r = 220, g = 140, b = 40; // orange = portrait
    if (orientation == EnemyOrientation::LANDSCAPE) {
        r = 60; g = 220; b = 80; // green = landscape
    }

    // Hit flash
    if (hitFlashTimer > 0.0f) {
        const float intensity = hitFlashTimer / kHitFlashDuration;
        r = static_cast<Uint8>(r + (255 - r) * intensity);
        g = static_cast<Uint8>(g + (255 - g) * intensity);
        b = static_cast<Uint8>(b + (255 - b) * intensity);
    }

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Orientation marker: small line showing "this way up"
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const float centerX = position.x;
    const float centerY = position.y;
    if (orientation == EnemyOrientation::LANDSCAPE) {
        // Horizontal line
        SDL_RenderLine(renderer, centerX - 10, centerY, centerX + 10, centerY);
    } else {
        // Vertical line
        SDL_RenderLine(renderer, centerX, centerY - 10, centerX, centerY + 10);
    }
}

SDL_FRect Enemy::GetBounds() const {
    const float width  = (orientation == EnemyOrientation::LANDSCAPE) ? 56.0f : 28.0f;
    const float height = (orientation == EnemyOrientation::LANDSCAPE) ? 28.0f : 56.0f;
    return SDL_FRect{
        position.x - width * 0.5f,
        position.y - height * 0.5f,
        width,
        height
    };
}

} // namespace rtz