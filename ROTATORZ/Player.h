#pragma once

#include <SDL3/SDL.h>
#include "GameObject.h"
#include "Vec2.h"

namespace rtz {

class Player final : public GameObject {
public:
    Player(Vec2 startPosition, float size);

    // Loads the ship sprite once (call from Game::Initialize). Falls back to a
    // coloured rectangle if loading fails or this isn't called.
    static bool LoadTexture(SDL_Renderer* renderer, const char* path);
    static void UnloadTexture();

    // Intent is supplied as world-X only (-1 = left, 1 = right, 0 = none).
    // The view rotation is purely visual, so the player never remaps input.
    void SetMoveIntent(float worldXDir);

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    SDL_FRect GetBounds() const override;

    Vec2 GetPosition() const { return position; }
    void SetPosition(const Vec2& pos) { position = pos; }

private:
    Vec2 position;
    float moveIntentX = 0.0f;
    float size = 36.0f;
    float speed = 320.0f;
    static SDL_Texture* s_texture;
};

} // namespace rtz
