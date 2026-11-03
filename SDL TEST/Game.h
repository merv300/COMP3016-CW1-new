#pragma once

#include <SDL3/SDL.h>

#include "Constants.h"
#include "InputManager.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "ObjectPool.h"

namespace rtz {

enum class GameState {
    Playing,
    GameOver
};

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
    void UpdatePlaying(float deltaTime);
    void UpdateGameOver(float deltaTime);

    void SpawnEnemy();
    void HandleCollisions();
    void ResetGame();
    void RenderUI();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* worldTexture = nullptr;

    InputManager input;
    Player player{Vec2{kWorldWidth * 0.5f, kWorldHeight - kPlayerSize * 0.5f}, kPlayerSize};

    ObjectPool<Bullet> bulletPool{128};
    ObjectPool<Enemy> enemyPool{64};

    GameState state = GameState::Playing;

    // Rotation: smooth tween between portrait (0°) and landscape (90° CW).
    float currentAngle = 0.0f;
    float targetAngle = 0.0f;

    bool running = true;

    // Gameplay stats
    int score = 0;
    int lives = 3;
    int wave = 1;
    float waveTimer = 0.0f;
    float spawnInterval = 2.0f;
    float respawnInvincibilityTimer = 0.0f;
    static constexpr float kRespawnInvincibilityTime = 2.0f;
    char nameBuffer[16] = {0};
    int nameLength = 0;
    bool nameEntryActive = false;
};

} // namespace rtz
