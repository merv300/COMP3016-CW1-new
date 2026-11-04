#include "Bullet.h"

#include "Constants.h"

namespace rtz {

void Bullet::Activate(const Vec2& startPos, const Vec2& direction, float speed, BulletType bulletType) {
    GameObject::Activate();
    position = startPos;
    velocity = direction * speed;
    type = bulletType;
    life = 0.0f;
}

void Bullet::Deactivate() {
    GameObject::Deactivate();
}

void Bullet::Update(float deltaTime) {
    position += velocity * deltaTime;
    life += deltaTime;

    // Despawn after max lifetime or if leaving world bounds (with margin).
    if (life >= kMaxLife ||
        position.x < -kRadius || position.x > kWorldWidth + kRadius ||
        position.y < -kRadius || position.y > kWorldHeight + kRadius) {
        Deactivate();
    }
}

void Bullet::Render(SDL_Renderer* renderer) {
    const SDL_FRect rect{
        position.x - kRadius,
        position.y - kRadius,
        kRadius * 2.0f,
        kRadius * 2.0f
    };

    if (type == BulletType::Player) {
        SDL_SetRenderDrawColor(renderer, 255, 230, 80, 255);   // yellow
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);    // red
    }
    SDL_RenderFillRect(renderer, &rect);
}

SDL_FRect Bullet::GetBounds() const {
    return SDL_FRect{
        position.x - kRadius,
        position.y - kRadius,
        kRadius * 2.0f,
        kRadius * 2.0f
    };
}

} // namespace rtz
