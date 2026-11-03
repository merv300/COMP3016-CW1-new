#pragma once

#include <SDL3/SDL.h>

#include "Constants.h"
#include "InputManager.h"
#include "Player.h"

namespace rtz {

// Owns the SDL window, renderer and the top-level game loop.
// The world is rendered into an off-screen texture which is then presented
// rotated, which is what makes the 90 degree view rotation cheap and exact.
class Game {
public:
    Game() = default;
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Runs until the window is closed. Throws std::runtime_error on failure.
    void Run();

private:
    void Initialize();
    void Shutdown();

    void PollEvents();
    void Update(float deltaTime);
    void Render();

    void DrawWorld();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* worldTexture = nullptr;

    InputManager input;
    Player player{Vec2{kWorldWidth * 0.5f, kWorldHeight * 0.5f}, kPlayerSize};

    int quarterTurns = 0;
    bool running = true;
};

} // namespace rtz
