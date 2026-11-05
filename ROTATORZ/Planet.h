#pragma once

#include <SDL3/SDL.h>

#ifdef RTZ_USE_SDL3_IMAGE
#include <SDL3_image/SDL_image.h>
#endif

#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

// Scrolling planet that drifts downward and wraps around the world, drawn
// behind the starfield as a parallax background layer. Purely cosmetic.
// Loads a PNG sprite if available, otherwise falls back to a coloured circle.
class Planet final : public GameObject {
public:
    // Activate arguments: x, y, size, speed, alpha
    void Activate(float x, float y, float sizeValue, float speedValue,
                  Uint8 alphaValue);

    void Deactivate() override;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void Render(SDL_Renderer* renderer, float xScale, float yScale);
    SDL_FRect GetBounds() const override;

    // Loads a planet sprite once per slot (call from Game::LoadAssets). Falls
    // back to a coloured circle if loading fails or this isn't called.
    static bool LoadTexture(SDL_Renderer* renderer, int slot, const char* path);
    static void UnloadTexture(int slot);
    static void UnloadAllTextures();

private:
    Vec2 position;
    float size = 1.0f;
    float speed = 0.0f;
    Uint8 alpha = 255;
    int textureSlot = 0; // which of the loaded planet sprites this one draws
    static SDL_Texture* s_textures[3];
};

} // namespace rtz
