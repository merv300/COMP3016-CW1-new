#pragma once

#include <SDL3/SDL.h>

#ifdef RTZ_USE_SDL3_MIXER
#include <SDL3_mixer/SDL_mixer.h>
#else
// Forward declarations so the header compiles without SDL3_mixer installed.
struct MIX_Mixer;
struct MIX_Audio;
struct MIX_Track;
#endif

#include "Constants.h"
#include "InputManager.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Bomb.h"
#include "Explosion.h"
#include "Star.h"
#include "ObjectPool.h"

namespace rtz {

enum class GameState {
    Menu,
    Playing,
    GameOver,
    Leaderboard
};

// Options on the Game Over screen.
enum class GameOverMenu {
    Retry,
    EnterName
};

// A single high-score table entry: player name plus score.
struct HighScoreEntry {
    char name[16] = {0};
    int score = 0;
};

// Owns the SDL window, renderer and the top-level game loop.
// The world is rendered into an off-screen texture which is then presented
// scaled and rotated to fit the window, which is what makes the 90 degree
// view rotation cheap and exact.
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
    void UpdateEnemyFire(float deltaTime);
    void UpdateGameOver(float deltaTime);

    void SpawnWave();
    void HandleCollisions();
    void UpdateBombs();
    void DetonateBomb(float centerX, float centerY);
    void DamagePlayer();
    void ResetGame();
    void LoadAssets();       // loads sprites, music and SFX
    void UnloadAssets();     // frees all loaded assets
    void RenderUI();
    void LoadHighScores();
    void SaveHighScores();
    void AddHighScore(const char* name, int score);
    void UpdateLeaderboard(float deltaTime);
    void RenderLeaderboard(int windowWidth, int windowHeight);
    void UpdateMenu(float deltaTime);
    void RenderMenu(int windowWidth, int windowHeight);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* worldTexture = nullptr;

    // Loaded assets
#ifdef RTZ_USE_SDL3_MIXER
    MIX_Mixer* mixer = nullptr;
    MIX_Audio* musicAudio = nullptr;
    MIX_Track* musicTrack = nullptr;
    MIX_Audio* shootAudio = nullptr;
    MIX_Audio* wrongHitAudio = nullptr;
#endif

    InputManager input;
    Player player{Vec2{kWorldWidth * 0.5f, kPlayerY}, kPlayerSize};

    ObjectPool<Bullet> bulletPool{128};
    ObjectPool<Bullet> enemyBulletPool{128};
    ObjectPool<Enemy> enemyPool{64};
    ObjectPool<Bomb> bombPool{32};
    ObjectPool<Explosion> explosionPool{16};
    ObjectPool<Star> starPool{kStarCount};

    GameState state = GameState::Menu;

    // Currently highlighted menu option (0=Start, 1=Leaderboard, 2=Exit).
    int menuSelected = 0;

    // Rotation: smooth tween between portrait (0°) and landscape (90° CW).
    float currentAngle = 0.0f;
    float targetAngle = 0.0f;

    bool running = true;
    bool isFullscreen = false;

    // Gameplay stats
    int score = 0;
    int lives = 3;
    int wave = 1;
    float waveClearTimer = 0.0f;
    float fireCooldown = 0.0f;
    float wrongHitSfxCooldown = 0.0f;
    float bombCooldown = kBombCooldownTime; // start ready
    float respawnInvincibilityTimer = 0.0f;
    static constexpr float kRespawnInvincibilityTime = kPlayerInvuln;
    char nameBuffer[16] = {0};
    int nameLength = 0;
    bool nameEntryActive = false;

    // Currently highlighted option on the Game Over screen.
    GameOverMenu gameOverMenu = GameOverMenu::Retry;

    // High score table (top 10), persisted to scores.dat.
    HighScoreEntry highScores[kMaxHighScores] = {};
    int highScoreCount = 0;
};

} // namespace rtz
