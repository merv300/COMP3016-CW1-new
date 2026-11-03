#include "Game.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include <stdexcept>
#include <string>

namespace rtz {

Game::~Game() {
    Shutdown();
}

void Game::Run() {
    Initialize();

    Uint64 previousCounter = SDL_GetPerformanceCounter();
    const double counterFrequency = static_cast<double>(SDL_GetPerformanceFrequency());

    while (running) {
        const Uint64 currentCounter = SDL_GetPerformanceCounter();
        float deltaTime = static_cast<float>((currentCounter - previousCounter) / counterFrequency);
        previousCounter = currentCounter;

        // Clamp the delta so a stall (breakpoint, alt-tab, slow first frame)
        // cannot move the player across the whole world in one step.
        if (deltaTime > kMaxFrameDelta) {
            deltaTime = kMaxFrameDelta;
        }

        PollEvents();
        if (!running) {
            break;
        }

        Update(deltaTime);
        Render();
    }
}

void Game::Initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer("ROTATORZ", kWindowWidth, kWindowHeight, 0, &window, &renderer)) {
        throw std::runtime_error(std::string("Window/renderer creation failed: ") + SDL_GetError());
    }

    // Syncing to the display keeps the delta-time loop from spinning at
    // hundreds of frames per second and wasting the CPU.
    SDL_SetRenderVSync(renderer, 1);

    worldTexture = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET,
                                     kWindowWidth,
                                     kWindowHeight);
    if (worldTexture == nullptr) {
        throw std::runtime_error(std::string("World texture creation failed: ") + SDL_GetError());
    }

    SDL_SetTextureScaleMode(worldTexture, SDL_SCALEMODE_LINEAR);

    ResetGame();
}

void Game::Shutdown() {
    if (worldTexture != nullptr) {
        SDL_DestroyTexture(worldTexture);
        worldTexture = nullptr;
    }
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

void Game::PollEvents() {
    input.BeginFrame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input.ProcessEvent(event);
    }

    input.EndFrame();

    if (input.QuitRequested()) {
        running = false;
    }
}

void Game::Update(float deltaTime) {
    if (input.WasKeyPressed(SDL_SCANCODE_ESCAPE)) {
        running = false;
    }

    if (state == GameState::Playing) {
        UpdatePlaying(deltaTime);
    } else {
        UpdateGameOver(deltaTime);
    }
}

void Game::UpdatePlaying(float deltaTime) {
    if (input.WasKeyPressed(SDL_SCANCODE_Q)) {
        targetAngle = (targetAngle == 0.0f) ? 90.0f : 0.0f;  // toggle portrait <-> landscape
    }

    // Smooth rotation tween toward target angle.
    float angleDiff = targetAngle - currentAngle;
    float step = kRotationSpeed * deltaTime;
    if (std::abs(angleDiff) <= step) {
        currentAngle = targetAngle;
    } else {
        currentAngle += (angleDiff > 0.0f ? step : -step);
    }

    // Player moves horizontally in world space only (A/D). Rotation is
    // purely visual — the world doesn't actually rotate, only the view.
    const float moveDir = input.GetMoveAxis().x;
    player.SetMoveIntent(moveDir);
    player.SetViewAngle(currentAngle);
    player.SetFiring(input.IsFiring());
    player.Update(deltaTime);

    // Lock player Y to bottom (portrait mode logic; rotation is visual only).
    Vec2 pos = player.GetPosition();
    pos.y = kWorldHeight - kPlayerSize * 0.5f;
    pos.x = std::clamp(pos.x, kPlayerSize * 0.5f, kWorldWidth - kPlayerSize * 0.5f);
    player.SetPosition(pos);

    // Fire bullets (always up in world space; rotation is visual only).
    static float fireCooldown = 0.0f;
    const float kFireRate = 0.12f;
    fireCooldown -= deltaTime;
    if (input.IsFiring() && fireCooldown <= 0.0f) {
        fireCooldown = kFireRate;
        const Vec2 forward{0.0f, -1.0f};
        const Vec2 spawnPos = player.GetPosition() + forward * (kPlayerSize * 0.6f);
        bulletPool.Acquire(spawnPos, forward, 600.0f);
    }

    // Update bullets
    bulletPool.ForEachActive([deltaTime](Bullet& b) { b.Update(deltaTime); });

    // Spawn enemies
    waveTimer += deltaTime;
    if (waveTimer >= spawnInterval) {
        waveTimer = 0.0f;
        SpawnEnemy();

        // Increase wave every 10 spawns
        static int spawnsThisWave = 0;
        spawnsThisWave++;
        if (spawnsThisWave >= 10) {
            spawnsThisWave = 0;
            wave++;
            spawnInterval = std::max(0.5f, spawnInterval * 0.85f);
        }
    }

    // Update enemies
    enemyPool.ForEachActive([deltaTime](Enemy& e) { e.Update(deltaTime); });

    // Collisions
    HandleCollisions();

    // Respawn invincibility
    if (respawnInvincibilityTimer > 0.0f) {
        respawnInvincibilityTimer -= deltaTime;
    }

    // Check game over
    if (lives <= 0) {
        state = GameState::GameOver;
        nameEntryActive = true;
        nameLength = 0;
    }
}

void Game::UpdateGameOver(float deltaTime) {
    // Name entry for high score
    if (nameEntryActive) {
        for (int sc = SDL_SCANCODE_A; sc <= SDL_SCANCODE_Z; ++sc) {
            if (input.WasKeyPressed(static_cast<SDL_Scancode>(sc))) {
                if (nameLength < 15) {
                    nameBuffer[nameLength++] = 'A' + (sc - SDL_SCANCODE_A);
                    nameBuffer[nameLength] = '\0';
                }
            }
        }
        if (input.WasKeyPressed(SDL_SCANCODE_BACKSPACE) && nameLength > 0) {
            nameBuffer[--nameLength] = '\0';
        }
        if (input.WasKeyPressed(SDL_SCANCODE_RETURN) || input.WasKeyPressed(SDL_SCANCODE_KP_ENTER)) {
            nameEntryActive = false;
            // TODO: save high score
        }
    }

    if (input.WasKeyPressed(SDL_SCANCODE_SPACE) || input.WasKeyPressed(SDL_SCANCODE_RETURN)) {
        if (!nameEntryActive) {
            ResetGame();
        }
    }
}

void Game::SpawnEnemy() {
    // Enemies always spawn from the top edge, moving down — rotation does
    // not affect where they enter from.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(50.0f, 750.0f);
    std::uniform_int_distribution<int> orientDist(0, 1);

    Vec2 spawnPos = {posDist(gen), -30.0f};
    Vec2 velocity = {0.0f, 80.0f + wave * 12.0f};

    const EnemyOrientation orientation = (orientDist(gen) == 0)
        ? EnemyOrientation::LANDSCAPE
        : EnemyOrientation::PORTRAIT;

    enemyPool.Acquire(spawnPos, velocity, orientation, wave);
}

void Game::HandleCollisions() {
    // Bullet vs Enemy
    bulletPool.ForEachActive([this](Bullet& bullet) {
        if (!bullet.IsActive()) return;

        enemyPool.ForEachActive([this, &bullet](Enemy& enemy) {
            if (!enemy.IsActive()) return;

            const SDL_FRect b = bullet.GetBounds();
            const SDL_FRect e = enemy.GetBounds();

            // AABB overlap test
            const bool overlap = !(b.x + b.w < e.x || e.x + e.w < b.x ||
                                   b.y + b.h < e.y || e.y + e.h < b.y);

            if (overlap) {
                // Only damage if current view rotation matches enemy orientation
                const int qt = (targetAngle == 90.0f) ? 1 : 0;
                if (enemy.IsVulnerableAt(qt)) {
                    bullet.Deactivate();
                    enemy.Deactivate();
                    score += enemy.GetScoreValue();
                    // Hit flash handled in enemy render via hitFlashTimer
                }
                // If not vulnerable, bullet passes through (no deactivation)
            }
        });
    });

    // Enemy vs Player (if not invincible)
    if (respawnInvincibilityTimer <= 0.0f) {
        const SDL_FRect p = player.GetBounds();
        enemyPool.ForEachActive([this, &p](Enemy& enemy) {
            if (!enemy.IsActive()) return;

            const SDL_FRect e = enemy.GetBounds();
            const bool overlap = !(p.x + p.w < e.x || e.x + e.w < p.x ||
                                   p.y + p.h < e.y || e.y + e.h < p.y);

            if (overlap) {
                enemy.Deactivate();
                lives--;
                respawnInvincibilityTimer = kRespawnInvincibilityTime;
                player.SetMoveIntent(0.0f);
                player.SetPosition({kWorldWidth * 0.5f, kWorldHeight - kPlayerSize * 0.5f});
            }
        });
    }
}

void Game::ResetGame() {
    score = 0;
    lives = 3;
    wave = 1;
    waveTimer = 0.0f;
    spawnInterval = 2.0f;
    respawnInvincibilityTimer = 0.0f;
    currentAngle = 0.0f;
    targetAngle = 0.0f;
    state = GameState::Playing;

    bulletPool.ReleaseAll();
    enemyPool.ReleaseAll();

    player.SetPosition({kWorldWidth * 0.5f, kWorldHeight - kPlayerSize * 0.5f});
    player.SetMoveIntent(0.0f);
    player.SetFiring(false);
    player.SetViewAngle(0.0f);
}

void Game::Render() {
    SDL_SetRenderTarget(renderer, worldTexture);
    DrawWorld();
    SDL_SetRenderTarget(renderer, nullptr);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const SDL_FRect destination{0.0f, 0.0f,
                                static_cast<float>(kWindowWidth),
                                static_cast<float>(kWindowHeight)};
    const SDL_FPoint center{static_cast<float>(kWindowWidth) * 0.5f,
                            static_cast<float>(kWindowHeight) * 0.5f};
    const double angle = currentAngle;

    SDL_RenderTextureRotated(renderer, worldTexture, nullptr, &destination, angle, &center, SDL_FLIP_NONE);

    // UI overlay (rendered after rotation so it stays screen-aligned)
    RenderUI();

    SDL_RenderPresent(renderer);
}

void Game::DrawWorld() {
    SDL_SetRenderDrawColor(renderer, 10, 12, 24, 255);
    SDL_RenderClear(renderer);

    // Reference grid so the current rotation is readable at a glance.
    SDL_SetRenderDrawColor(renderer, 28, 34, 58, 255);
    for (int x = 0; x <= kWindowWidth; x += 100) {
        SDL_RenderLine(renderer, static_cast<float>(x), 0.0f,
                       static_cast<float>(x), kWorldHeight);
    }
    for (int y = 0; y <= kWindowHeight; y += 100) {
        SDL_RenderLine(renderer, 0.0f, static_cast<float>(y),
                       kWorldWidth, static_cast<float>(y));
    }

    // Marker pinned to the world's top-left corner.
    SDL_SetRenderDrawColor(renderer, 220, 70, 90, 255);
    const SDL_FRect marker{20.0f, 20.0f, 60.0f, 60.0f};
    SDL_RenderFillRect(renderer, &marker);

    // Bullets
    bulletPool.ForEachActive([this](Bullet& b) { b.Render(renderer); });

    // Enemies
    enemyPool.ForEachActive([this](Enemy& e) { e.Render(renderer); });

    // Player (render last so on top)
    if (respawnInvincibilityTimer <= 0.0f || static_cast<int>(respawnInvincibilityTimer * 10) % 2 == 0) {
        player.Render(renderer);
    }
}

void Game::RenderUI() {
    // Score
    char buf[64];
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Draw score text using simple rects (placeholder for real font)
    snprintf(buf, sizeof(buf), "SCORE: %d", score);
    // TODO: Use SDL3_ttf for real text. For now draw a bar.
    SDL_FRect scoreBar{10.0f, 10.0f, static_cast<float>(std::min<int>(score / 10, 300)), 24.0f};
    SDL_SetRenderDrawColor(renderer, 80, 200, 120, 255);
    SDL_RenderFillRect(renderer, &scoreBar);

    // Lives
    for (int i = 0; i < lives; ++i) {
        SDL_FRect lifeIcon{10.0f + i * 28.0f, 40.0f, 20.0f, 20.0f};
        SDL_SetRenderDrawColor(renderer, 120, 220, 255, 255);
        SDL_RenderFillRect(renderer, &lifeIcon);
    }

    // Wave
    snprintf(buf, sizeof(buf), "WAVE: %d", wave);
    // Draw wave indicator
    SDL_FRect waveBar{600.0f, 10.0f, static_cast<float>(wave * 30), 16.0f};
    SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
    SDL_RenderFillRect(renderer, &waveBar);

    // Rotation indicator
    const char* rotNames[4] = {"0°", "90°", "180°", "270°"};
    // TODO: draw rotation indicator

    // Game Over screen
    if (state == GameState::GameOver) {
        // Semi-transparent overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_FRect overlay{0, 0, 800.0f, 800.0f};
        SDL_RenderFillRect(renderer, &overlay);

        // Game Over box
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        SDL_FRect box{200.0f, 250.0f, 400.0f, 300.0f};
        SDL_RenderFillRect(renderer, &box);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &box);

        if (nameEntryActive) {
            // Name entry
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect nameBox{250.0f, 350.0f, 300.0f, 40.0f};
            SDL_RenderRect(renderer, &nameBox);
            // Cursor blink
            if (static_cast<int>(SDL_GetTicks() / 500) % 2 == 0) {
                // Would draw nameBuffer + '_'
            }
        } else {
            // Press Space to restart
        }
    }
}

} // namespace rtz