#include "Explosion.h"

#include <algorithm>
#include <cmath>

namespace rtz {

namespace {

// SDL3's base renderer has no circle primitive, so a filled circle is
// approximated by stacking 1-pixel-tall horizontal bars.
void DrawFilledCircle(SDL_Renderer* renderer, float cx, float cy, float radius,
                      Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
    if (radius < 0.5f) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);

    const int steps = static_cast<int>(std::ceil(radius));
    for (int dy = -steps; dy <= steps; ++dy) {
        const float yy = static_cast<float>(dy);
        const float halfWidth = std::sqrt(std::max(0.0f, radius * radius - yy * yy));
        if (halfWidth <= 0.0f) {
            continue;
        }

        SDL_FRect line{cx - halfWidth, cy + yy, halfWidth * 2.0f, 1.0f};
        SDL_RenderFillRect(renderer, &line);
    }
}

} // namespace

void Explosion::Activate(const Vec2& centre) {
    GameObject::Activate();
    center = centre;
    timer = kDuration;
}

void Explosion::Deactivate() {
    GameObject::Deactivate();
}

void Explosion::Update(float deltaTime) {
    timer -= deltaTime;
    if (timer <= 0.0f) {
        Deactivate();
    }
}

void Explosion::Render(SDL_Renderer* renderer) {
    // progress runs 0 → 1 over the explosion's lifetime.
    const float progress = 1.0f - (timer / kDuration);
    const float radius = kExplosionRadius * (0.3f + 0.7f * progress);
    const Uint8 alpha = static_cast<Uint8>(255.0f * (1.0f - progress));

    // Outer fiery ring (dim, large) then inner hot core (bright, smaller).
    DrawFilledCircle(renderer, center.x, center.y, radius,
                     255, 120, 30, static_cast<Uint8>(alpha / 2));
    DrawFilledCircle(renderer, center.x, center.y, radius * 0.55f,
                     255, 240, 180, alpha);
}

SDL_FRect Explosion::GetBounds() const {
    return SDL_FRect{
        center.x - kExplosionRadius,
        center.y - kExplosionRadius,
        kExplosionRadius * 2.0f,
        kExplosionRadius * 2.0f
    };
}

} // namespace rtz
