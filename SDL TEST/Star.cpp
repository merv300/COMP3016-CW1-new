#include "Star.h"

#include <cstdlib>

#include "Constants.h"

namespace rtz {

void Star::Activate(float x, float y, float sizeValue, float speedValue,
                    Uint8 alphaValue) {
    GameObject::Activate();
    position = {x, y};
    size = sizeValue;
    speed = speedValue;
    alpha = alphaValue;
}

void Star::Deactivate() {
    GameObject::Deactivate();
}

void Star::Update(float deltaTime) {
    position.y += speed * deltaTime;

    // Wrap around to the top when the star leaves the bottom of the world.
    if (position.y > kWorldHeight) {
        position.y = -size;
        position.x =
            static_cast<float>(std::rand() % static_cast<int>(kWorldWidth));
    }
}

void Star::Render(SDL_Renderer* renderer) {
    SDL_FRect rect{position.x, position.y, size, size};
    SDL_SetRenderDrawColor(renderer, 220, 220, 255, alpha);
    SDL_RenderFillRect(renderer, &rect);
}

SDL_FRect Star::GetBounds() const {
    return SDL_FRect{position.x, position.y, size, size};
}

} // namespace rtz
