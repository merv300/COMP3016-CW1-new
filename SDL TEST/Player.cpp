#include "Player.h"

#include <algorithm>

#include "Constants.h"

namespace rtz {

Player::Player(Vec2 startPosition, float playerSize)
    : position(startPosition), size(playerSize), speed(kPlayerSpeed) {}

void Player::SetMoveIntent(const Vec2& worldDirection) {
    moveDirection = worldDirection.Normalized();
}

void Player::Update(float deltaTime) {
    // Delta-time integration: speed is expressed in pixels per second, so
    // movement is independent of the frame rate.
    position += moveDirection * (speed * deltaTime);

    const float half = size * 0.5f;
    position.x = std::clamp(position.x, half, kWorldWidth - half);
    position.y = std::clamp(position.y, half, kWorldHeight - half);
}

void Player::Render(SDL_Renderer* renderer) {
    SDL_FRect body = GetBounds();
    SDL_SetRenderDrawColor(renderer, 120, 220, 255, 255);
    SDL_RenderFillRect(renderer, &body);

    // The ship always points towards screen-up. Converting that direction into
    // world space makes the rotation visible when the view turns.
    const Vec2 nose = WorldFromScreen({0.0f, -1.0f}, facingQuarterTurns);
    const float centerX = position.x;
    const float centerY = position.y;
    const float noseLength = size * 0.7f;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderLine(renderer, centerX, centerY,
                   centerX + nose.x * noseLength,
                   centerY + nose.y * noseLength);

    if (firing) {
        SDL_SetRenderDrawColor(renderer, 255, 214, 102, 255);
        SDL_RenderLine(renderer, centerX + nose.x * noseLength, centerY + nose.y * noseLength,
                       centerX + nose.x * (noseLength + 14.0f),
                       centerY + nose.y * (noseLength + 14.0f));
    }
}

SDL_FRect Player::GetBounds() const {
    return SDL_FRect{position.x - size * 0.5f, position.y - size * 0.5f, size, size};
}

} // namespace rtz
