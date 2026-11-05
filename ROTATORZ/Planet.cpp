#include "Planet.h"

#include <cmath>
#include <cstdlib>

#include "Constants.h"

namespace rtz {

SDL_Texture* Planet::s_textures[3] = {nullptr, nullptr, nullptr};

bool Planet::LoadTexture(SDL_Renderer* renderer, int slot, const char* path) {
#ifdef RTZ_USE_SDL3_IMAGE
    UnloadTexture(slot);
    SDL_Surface* surf = IMG_Load(path);
    if (surf == nullptr) {
        return false;
    }
    s_textures[slot] = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    return s_textures[slot] != nullptr;
#else
    (void)renderer; (void)slot; (void)path;
    return false;
#endif
}

void Planet::UnloadTexture(int slot) {
    if (s_textures[slot] != nullptr) {
        SDL_DestroyTexture(s_textures[slot]);
        s_textures[slot] = nullptr;
    }
}

void Planet::UnloadAllTextures() {
    for (int i = 0; i < 3; ++i) {
        UnloadTexture(i);
    }
}

void Planet::Activate(float x, float y, float sizeValue, float speedValue,
                      Uint8 alphaValue) {
    GameObject::Activate();
    position = {x, y};
    size = sizeValue;
    speed = speedValue;
    alpha = alphaValue;
    textureSlot = std::rand() % 3;
}

void Planet::Deactivate() {
    GameObject::Deactivate();
}

void Planet::Update(float deltaTime) {
    position.y += speed * deltaTime;

    // Wrap around to the top when the planet leaves the bottom of the world.
    if (position.y - size > kWorldHeight) {
        position.y = -size;
        position.x =
            static_cast<float>(std::rand() % static_cast<int>(kWorldWidth));
    }
}

void Planet::Render(SDL_Renderer* renderer) {
    Render(renderer, 1.0f, 1.0f);
}

void Planet::Render(SDL_Renderer* renderer, float xScale, float yScale) {
    if (s_textures[textureSlot] != nullptr) {
        SDL_SetTextureColorMod(s_textures[textureSlot], 255, 255, 255);
        SDL_FRect rect{position.x * xScale - size * 0.5f,
                       position.y * yScale - size * 0.5f,
                       size * xScale, size * yScale};
        SDL_RenderTexture(renderer, s_textures[textureSlot], nullptr, &rect);
        return;
    }

    // Fallback: draw a coloured circle so planets are visible without assets.
    const int segments = 24;
    SDL_Vertex vertices[segments + 2];
    SDL_FColor color{220.0f / 255.0f, 200.0f / 255.0f, 160.0f / 255.0f,
                     alpha / 255.0f};
    vertices[0].position = {position.x * xScale, position.y * yScale};
    vertices[0].color = color;
    vertices[0].tex_coord = {0.0f, 0.0f};
    const float radius = size * 0.5f;
    for (int i = 0; i <= segments; ++i) {
        const float angle = (2.0f * 3.14159265f * i) / segments;
        vertices[i + 1].position = {position.x * xScale +
                                    std::cos(angle) * radius,
                                    position.y * yScale +
                                    std::sin(angle) * radius};
        vertices[i + 1].color = color;
        vertices[i + 1].tex_coord = {0.0f, 0.0f};
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(renderer, nullptr, vertices, segments + 2, nullptr, 0);
}

SDL_FRect Planet::GetBounds() const {
    return SDL_FRect{position.x - size * 0.5f, position.y - size * 0.5f, size,
                     size};
}

} // namespace rtz
