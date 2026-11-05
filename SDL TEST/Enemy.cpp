#include "Enemy.h"

#include "Constants.h"

#ifdef RTZ_USE_SDL3_IMAGE
#include <SDL3_image/SDL_image.h>
#endif

namespace rtz {

SDL_Texture* Enemy::s_portraitTexture = nullptr;
SDL_Texture* Enemy::s_landscapeTexture = nullptr;

bool Enemy::LoadTextures(SDL_Renderer* renderer,
                         const char* portraitPath,
                         const char* landscapePath) {
#ifdef RTZ_USE_SDL3_IMAGE
    UnloadTextures();

    SDL_Surface* surf = IMG_Load(portraitPath);
    if (surf != nullptr) {
        s_portraitTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);
    }

    surf = IMG_Load(landscapePath);
    if (surf != nullptr) {
        s_landscapeTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);
    }

    return s_portraitTexture != nullptr || s_landscapeTexture != nullptr;
#else
    (void)renderer; (void)portraitPath; (void)landscapePath;
    return false;
#endif
}

void Enemy::UnloadTextures() {
    if (s_portraitTexture != nullptr) {
        SDL_DestroyTexture(s_portraitTexture);
        s_portraitTexture = nullptr;
    }
    if (s_landscapeTexture != nullptr) {
        SDL_DestroyTexture(s_landscapeTexture);
        s_landscapeTexture = nullptr;
    }
}

void Enemy::Activate(const Vec2& startPos, EnemyOrientation orient, int waveNumber) {
    GameObject::Activate();
    position = startPos;
    orientation = orient;
    wave = waveNumber;
    hitFlashTimer = 0.0f;
    shootTimer = kEnemyShootMin; // Game re-randomises this right after acquire
    descendSpeed = kEnemyDescendSpeed;

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
    // The whole formation descends towards the player.
    position.y += descendSpeed * deltaTime;

    if (hitFlashTimer > 0.0f) {
        hitFlashTimer -= deltaTime;
    }

    // Deactivate once the enemy has fully left the world.
    const float margin = 60.0f;
    if (position.x < -margin || position.x > kWorldWidth + margin ||
        position.y - kEnemyH * 0.5f > kWorldHeight + margin) {
        Deactivate();
    }
}

void Enemy::Render(SDL_Renderer* renderer) {
    const SDL_FRect rect = GetBounds();

    // Pick the sprite for this enemy's orientation.
    SDL_Texture* tex = (orientation == EnemyOrientation::LANDSCAPE)
                           ? s_landscapeTexture
                           : s_portraitTexture;

    // Base colour by orientation (for the fallback rect and tinting)
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

    if (tex != nullptr) {
        SDL_SetTextureColorMod(tex, r, g, b);
        SDL_RenderTexture(renderer, tex, nullptr, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    // Orientation marker: a small line hinting at the axis it is aligned to.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const float centerX = position.x;
    const float centerY = position.y;
    if (orientation == EnemyOrientation::LANDSCAPE) {
        SDL_RenderLine(renderer, centerX - 10.0f, centerY, centerX + 10.0f, centerY);
    } else {
        SDL_RenderLine(renderer, centerX, centerY - 10.0f, centerX, centerY + 10.0f);
    }
}

SDL_FRect Enemy::GetBounds() const {
    return SDL_FRect{
        position.x - kEnemyW * 0.5f,
        position.y - kEnemyH * 0.5f,
        kEnemyW,
        kEnemyH
    };
}

} // namespace rtz
