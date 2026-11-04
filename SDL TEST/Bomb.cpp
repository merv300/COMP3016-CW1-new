#include "Bomb.h"

#include <cmath>

#include "Constants.h"

namespace rtz {

void Bomb::Activate(const Vec2& startPos, const Vec2& vel) {
    GameObject::Activate();
    position = startPos;
    velocity = vel;
    life = 0.0f;
}

void Bomb::Deactivate() {
    GameObject::Deactivate();
}

void Bomb::Update(float deltaTime) {
    position += velocity * deltaTime;
    life += deltaTime;

    // Despawn after max lifetime or once it leaves the top of the world.
    if (life >= kMaxLife ||
        position.y + kBombSize * 0.5f < -kBombSize) {
        Deactivate();
    }
}

void Bomb::Render(SDL_Renderer* renderer) {
    // Pulsing fireball: outer glow, mid glow, hot core.
    const float pulse = 0.85f + 0.15f * std::sin(life * 15.0f);
    const float baseRadius = kBombSize * 0.5f * pulse;

    SDL_FRect outer{
        position.x - baseRadius * 2.2f,
        position.y - baseRadius * 2.2f,
        baseRadius * 4.4f,
        baseRadius * 4.4f
    };
    SDL_SetRenderDrawColor(renderer, 255, 100, 40, 100);
    SDL_RenderFillRect(renderer, &outer);

    SDL_FRect mid{
        position.x - baseRadius * 1.3f,
        position.y - baseRadius * 1.3f,
        baseRadius * 2.6f,
        baseRadius * 2.6f
    };
    SDL_SetRenderDrawColor(renderer, 255, 160, 60, 200);
    SDL_RenderFillRect(renderer, &mid);

    SDL_FRect core{
        position.x - baseRadius * 0.6f,
        position.y - baseRadius * 0.6f,
        baseRadius * 1.2f,
        baseRadius * 1.2f
    };
    SDL_SetRenderDrawColor(renderer, 255, 240, 140, 255);
    SDL_RenderFillRect(renderer, &core);
}

SDL_FRect Bomb::GetBounds() const {
    return SDL_FRect{
        position.x - kBombSize * 0.5f,
        position.y - kBombSize * 0.5f,
        kBombSize,
        kBombSize
    };
}

} // namespace rtz
