#include "Player.h"

#include <algorithm>

#include "Constants.h"

namespace rtz {

Player::Player(Vec2 startPosition, float playerSize)
    : position(startPosition), size(playerSize), speed(kPlayerSpeed) {}

void Player::SetMoveIntent(float worldXDir) {
    moveIntentX = worldXDir;
}

void Player::Update(float deltaTime) {
    // Delta-time integration: speed is expressed in pixels per second.
    // Movement is horizontal-only in world space; Y is locked by Game.
    position.x += moveIntentX * (speed * deltaTime);

    const float half = size * 0.5f;
    position.x = std::clamp(position.x, half, kWorldWidth - half);
}

void Player::Render(SDL_Renderer* renderer) {
    SDL_FRect body = GetBounds();
    SDL_SetRenderDrawColor(renderer, 120, 220, 255, 255);
    SDL_RenderFillRect(renderer, &body);

    // Nose indicator points to screen-up (-Y in world space). It is rotated
    // visually by the view angle so it always points "up" on the rotated screen.
    const float rad = viewAngle * 3.14159265f / 180.0f;
    const float noseX = std::sin(rad) * (size * 0.7f);
    const float noseY = -std::cos(rad) * (size * 0.7f);
    const float centerX = position.x;
    const float centerY = position.y;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderLine(renderer, centerX, centerY,
                   centerX + noseX,
                   centerY + noseY);

    if (firing) {
        SDL_SetRenderDrawColor(renderer, 255, 214, 102, 255);
        SDL_RenderLine(renderer, centerX + noseX, centerY + noseY,
                       centerX + noseX * 1.5f,
                       centerY + noseY * 1.5f);
    }
}

SDL_FRect Player::GetBounds() const {
    return SDL_FRect{position.x - size * 0.5f, position.y - size * 0.5f, size, size};
}

} // namespace rtz