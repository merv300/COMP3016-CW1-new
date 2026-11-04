#include "Game.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <random>
#include <stdexcept>
#include <string>

namespace rtz {

namespace {

// Single shared generator so wave spawns and shoot timers stay random without
// re-seeding a device on every call.
std::mt19937& Rng() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

float RandomRange(float low, float high) {
    std::uniform_real_distribution<float> distribution(low, high);
    return distribution(Rng());
}

} // namespace

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

    // Needed for the bomb glow and explosion fade (alpha blending).
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // The world texture is the fixed playfield size, not the window size.
    worldTexture = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET,
                                     static_cast<int>(kWorldWidth),
                                     static_cast<int>(kWorldHeight));
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

    if (input.WasKeyPressed(SDL_SCANCODE_F11)) {
        isFullscreen = !isFullscreen;
        SDL_SetWindowFullscreen(window, isFullscreen);
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
    const float angleDiff = targetAngle - currentAngle;
    const float step = kRotationSpeed * deltaTime;
    if (std::abs(angleDiff) <= step) {
        currentAngle = targetAngle;
    } else {
        currentAngle += (angleDiff > 0.0f ? step : -step);
    }

    // Player moves horizontally in world space only (A/D). Rotation is
    // purely visual — the world doesn't actually rotate, only the view.
    player.SetMoveIntent(input.GetMoveAxis().x);
    player.SetViewAngle(currentAngle);
    player.SetFiring(input.IsFiring());
    player.Update(deltaTime);

    // Lock the player near the bottom edge and keep the whole ship on-screen.
    Vec2 pos = player.GetPosition();
    pos.y = kPlayerY;
    const float half = kPlayerSize * 0.5f;
    pos.x = std::clamp(pos.x, kPlayerMargin + half, kWorldWidth - kPlayerMargin - half);
    player.SetPosition(pos);

    // Player fire (always up in world space; rotation is visual only).
    fireCooldown -= deltaTime;
    if (input.IsFiring() && fireCooldown <= 0.0f) {
        fireCooldown = kFireCooldown;
        const Vec2 forward{0.0f, -1.0f};
        const Vec2 spawnPos = player.GetPosition() + forward * (kPlayerSize * 0.6f);
        bulletPool.Acquire(spawnPos, forward, kPlayerBulletSpeed, BulletType::Player);
    }

    // Bomb cooldown tick (starts fully charged on a fresh game).
    if (bombCooldown < kBombCooldownTime) {
        bombCooldown += deltaTime;
    }

    // Bomb fire: right-click while the cooldown is fully charged.
    if (input.IsRightMouseDown() && bombCooldown >= kBombCooldownTime) {
        bombCooldown = 0.0f;
        const Vec2 spawnPos = player.GetPosition() +
                              Vec2{0.0f, -kPlayerSize * 0.5f - kBombSize * 0.5f};
        bombPool.Acquire(spawnPos, Vec2{0.0f, -kBombSpeed});
    }

    // Update bullets.
    bulletPool.ForEachActive([deltaTime](Bullet& bullet) { bullet.Update(deltaTime); });

    // Front-line enemies fire down at the player.
    UpdateEnemyFire(deltaTime);
    enemyBulletPool.ForEachActive([deltaTime](Bullet& bullet) { bullet.Update(deltaTime); });

    // The whole formation descends.
    enemyPool.ForEachActive([deltaTime](Enemy& enemy) { enemy.Update(deltaTime); });

    // Bombs move, then detonate on contact; explosions fade out.
    bombPool.ForEachActive([deltaTime](Bomb& bomb) { bomb.Update(deltaTime); });
    UpdateBombs();
    explosionPool.ForEachActive([deltaTime](Explosion& explosion) { explosion.Update(deltaTime); });

    HandleCollisions();

    // Once the field is clear, wait a beat and send in the next wave.
    if (enemyPool.ActiveCount() == 0) {
        waveClearTimer -= deltaTime;
        if (waveClearTimer <= 0.0f) {
            ++wave;
            SpawnWave();
        }
    }

    // Respawn invincibility.
    if (respawnInvincibilityTimer > 0.0f) {
        respawnInvincibilityTimer -= deltaTime;
    }

    // Check game over.
    if (lives <= 0 && state == GameState::Playing) {
        state = GameState::GameOver;
        nameEntryActive = true;
        nameLength = 0;
        nameBuffer[0] = '\0';
    }
}

void Game::UpdateEnemyFire(float deltaTime) {
    enemyPool.ForEachActive([this, deltaTime](Enemy& enemy) {
        if (!enemy.IsActive()) {
            return;
        }

        // Only the lowest enemy in each column may fire, so the player can
        // read the threat as a clear front line.
        bool isFrontLine = true;
        enemyPool.ForEachActive([&enemy, &isFrontLine](Enemy& other) {
            if (!other.IsActive() || &other == &enemy) {
                return;
            }
            const Vec2 self = enemy.GetPosition();
            const Vec2 candidate = other.GetPosition();
            if (std::abs(candidate.x - self.x) < kEnemyW * 0.5f &&
                candidate.y > self.y) {
                isFrontLine = false;
            }
        });

        if (!isFrontLine) {
            return;
        }

        enemy.SetShootTimer(enemy.GetShootTimer() - deltaTime);
        if (enemy.GetShootTimer() <= 0.0f) {
            enemy.SetShootTimer(RandomRange(kEnemyShootMin, kEnemyShootMax));

            const Vec2 origin = enemy.GetPosition();
            const Vec2 spawnPos{origin.x, origin.y + kEnemyH * 0.5f};
            enemyBulletPool.Acquire(spawnPos, Vec2{0.0f, 1.0f},
                                    kEnemyBulletSpeed, BulletType::Enemy);
        }
    });
}

void Game::UpdateBombs() {
    bombPool.ForEachActive([this](Bomb& bomb) {
        if (!bomb.IsActive()) {
            return;
        }

        // Bomb detonates on contact with any enemy, regardless of colour.
        const SDL_FRect bombBounds = bomb.GetBounds();
        bool detonated = false;

        enemyPool.ForEachActive([this, &bomb, &bombBounds, &detonated](Enemy& enemy) {
            if (detonated || !enemy.IsActive()) {
                return;
            }

            const SDL_FRect e = enemy.GetBounds();
            const bool overlap = !(bombBounds.x + bombBounds.w < e.x || e.x + e.w < bombBounds.x ||
                                   bombBounds.y + bombBounds.h < e.y || e.y + e.h < bombBounds.y);
            if (!overlap) {
                return;
            }

            const Vec2 origin = bomb.GetPosition();
            bomb.Deactivate();
            detonated = true;
            DetonateBomb(origin.x, origin.y);
        });
    });
}

void Game::DetonateBomb(float centerX, float centerY) {
    // Visual blast.
    explosionPool.Acquire(Vec2{centerX, centerY});

    // The bomb deliberately ignores orientation: every enemy inside the
    // radius dies, which is what makes it a panic button.
    enemyPool.ForEachActive([this, centerX, centerY](Enemy& enemy) {
        if (!enemy.IsActive()) {
            return;
        }

        const Vec2 position = enemy.GetPosition();
        const float dx = position.x - centerX;
        const float dy = position.y - centerY;
        if (dx * dx + dy * dy <= kExplosionRadius * kExplosionRadius) {
            enemy.Deactivate();
            score += static_cast<int>(kBombScore);
        }
    });
}

void Game::UpdateGameOver(float deltaTime) {
    (void)deltaTime;

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

void Game::SpawnWave() {
    // 6 columns x 4 rows, centred horizontally. Rows alternate orientation so
    // the player has to flip the view to clear the whole formation.
    const float totalWidth = kEnemyCols * kEnemyW + (kEnemyCols - 1) * kEnemyGapX;
    const float startX = (kWorldWidth - totalWidth) * 0.5f;

    for (int row = 0; row < kEnemyRows; ++row) {
        for (int col = 0; col < kEnemyCols; ++col) {
            const Vec2 centre{
                startX + col * (kEnemyW + kEnemyGapX) + kEnemyW * 0.5f,
                kEnemyStartY + row * (kEnemyH + kEnemyGapY) + kEnemyH * 0.5f
            };

            const EnemyOrientation orientation =
                (row % 2 == 1) ? EnemyOrientation::LANDSCAPE
                               : EnemyOrientation::PORTRAIT;

            Enemy* enemy = enemyPool.Acquire(centre, orientation, wave);
            enemy->SetShootTimer(RandomRange(kEnemyShootMin, kEnemyShootMax));
        }
    }

    waveClearTimer = kWaveClearDelay;
}

void Game::HandleCollisions() {
    // The game only has two view states, so the target angle maps directly
    // onto the quarter-turn index used by Enemy::IsVulnerableAt.
    const int quarterTurns = (targetAngle == 90.0f) ? 1 : 0;

    // Player bullets vs enemies (orientation-gated).
    bulletPool.ForEachActive([this, quarterTurns](Bullet& bullet) {
        if (!bullet.IsActive()) {
            return;
        }

        enemyPool.ForEachActive([this, quarterTurns, &bullet](Enemy& enemy) {
            if (!bullet.IsActive() || !enemy.IsActive()) {
                return;
            }

            const SDL_FRect b = bullet.GetBounds();
            const SDL_FRect e = enemy.GetBounds();
            const bool overlap = !(b.x + b.w < e.x || e.x + e.w < b.x ||
                                   b.y + b.h < e.y || e.y + e.h < b.y);

            if (!overlap) {
                return;
            }

            // Bullets never pass through an enemy. The shot is consumed on
            // contact regardless of orientation; only a matching orientation
            // actually destroys the enemy. This prevents shooting "through"
            // a wrong-coloured enemy to hit a vulnerable one behind it.
            bullet.Deactivate();

            if (enemy.IsVulnerableAt(quarterTurns)) {
                enemy.Deactivate();
                score += enemy.GetScoreValue();
            }
        });
    });

    // Enemy bullets vs player.
    if (respawnInvincibilityTimer <= 0.0f) {
        const SDL_FRect p = player.GetBounds();
        enemyBulletPool.ForEachActive([this, &p](Bullet& bullet) {
            if (!bullet.IsActive()) {
                return;
            }

            const SDL_FRect b = bullet.GetBounds();
            const bool overlap = !(p.x + p.w < b.x || b.x + b.w < p.x ||
                                   p.y + p.h < b.y || b.y + b.h < p.y);
            if (!overlap) {
                return;
            }

            bullet.Deactivate();
            DamagePlayer();
        });
    }

    // Enemy vs player contact.
    if (respawnInvincibilityTimer <= 0.0f) {
        const SDL_FRect p = player.GetBounds();
        enemyPool.ForEachActive([this, &p](Enemy& enemy) {
            if (!enemy.IsActive()) {
                return;
            }

            const SDL_FRect e = enemy.GetBounds();
            const bool overlap = !(p.x + p.w < e.x || e.x + e.w < p.x ||
                                   p.y + p.h < e.y || e.y + e.h < p.y);
            if (!overlap) {
                return;
            }

            enemy.Deactivate();
            DamagePlayer();
        });
    }
}

void Game::DamagePlayer() {
    if (respawnInvincibilityTimer > 0.0f) {
        return;
    }

    --lives;
    respawnInvincibilityTimer = kRespawnInvincibilityTime;
    player.SetMoveIntent(0.0f);
    player.SetPosition({kWorldWidth * 0.5f, kPlayerY});
}

void Game::ResetGame() {
    score = 0;
    lives = 3;
    wave = 1;
    waveClearTimer = kWaveClearDelay;
    fireCooldown = 0.0f;
    bombCooldown = kBombCooldownTime;
    respawnInvincibilityTimer = 0.0f;
    currentAngle = 0.0f;
    targetAngle = 0.0f;
    state = GameState::Playing;
    nameEntryActive = false;
    nameLength = 0;
    nameBuffer[0] = '\0';

    bulletPool.ReleaseAll();
    enemyBulletPool.ReleaseAll();
    enemyPool.ReleaseAll();
    bombPool.ReleaseAll();
    explosionPool.ReleaseAll();

    player.SetPosition({kWorldWidth * 0.5f, kPlayerY});
    player.SetMoveIntent(0.0f);
    player.SetFiring(false);
    player.SetViewAngle(0.0f);

    SpawnWave();
}

void Game::Render() {
    // 1. Draw the world into the fixed-size off-screen texture.
    SDL_SetRenderTarget(renderer, worldTexture);
    DrawWorld();
    SDL_SetRenderTarget(renderer, nullptr);

    // 2. Present that texture scaled to fit the window and rotated about its
    //    centre. The AABB of the rotated world determines the scale, so the
    //    whole playfield always stays visible at either orientation.
    //    Querying the window each frame keeps the layout correct after an
    //    F11 fullscreen toggle changes the available size.
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const float radians = currentAngle * 3.14159265f / 180.0f;
    const float cosine = std::abs(std::cos(radians));
    const float sine = std::abs(std::sin(radians));

    const float aabbWidth = kWorldWidth * cosine + kWorldHeight * sine;
    const float aabbHeight = kWorldWidth * sine + kWorldHeight * cosine;
    const float scale = std::min(static_cast<float>(windowWidth) / aabbWidth,
                                 static_cast<float>(windowHeight) / aabbHeight);

    const float drawWidth = kWorldWidth * scale;
    const float drawHeight = kWorldHeight * scale;
    const SDL_FRect destination{
        (static_cast<float>(windowWidth) - drawWidth) * 0.5f,
        (static_cast<float>(windowHeight) - drawHeight) * 0.5f,
        drawWidth,
        drawHeight
    };

    SDL_RenderTextureRotated(renderer, worldTexture, nullptr, &destination,
                             currentAngle, nullptr, SDL_FLIP_NONE);

    // 3. UI overlay (rendered after rotation so it stays screen-aligned).
    RenderUI();

    SDL_RenderPresent(renderer);
}

void Game::DrawWorld() {
    SDL_SetRenderDrawColor(renderer, 10, 12, 24, 255);
    SDL_RenderClear(renderer);

    // Reference grid so the current rotation is readable at a glance.
    SDL_SetRenderDrawColor(renderer, 28, 34, 58, 255);
    for (float x = 0.0f; x <= kWorldWidth; x += 60.0f) {
        SDL_RenderLine(renderer, x, 0.0f, x, kWorldHeight);
    }
    for (float y = 0.0f; y <= kWorldHeight; y += 60.0f) {
        SDL_RenderLine(renderer, 0.0f, y, kWorldWidth, y);
    }

    // Marker pinned to the world's top-left corner.
    SDL_SetRenderDrawColor(renderer, 220, 70, 90, 255);
    const SDL_FRect marker{20.0f, 20.0f, 40.0f, 40.0f};
    SDL_RenderFillRect(renderer, &marker);

    // Bullets
    bulletPool.ForEachActive([this](Bullet& bullet) { bullet.Render(renderer); });
    enemyBulletPool.ForEachActive([this](Bullet& bullet) { bullet.Render(renderer); });

    // Bombs
    bombPool.ForEachActive([this](Bomb& bomb) { bomb.Render(renderer); });

    // Enemies
    enemyPool.ForEachActive([this](Enemy& enemy) { enemy.Render(renderer); });

    // Explosions sit above enemies so kills read clearly.
    explosionPool.ForEachActive([this](Explosion& explosion) { explosion.Render(renderer); });

    // Player (render last so it sits on top)
    if (respawnInvincibilityTimer <= 0.0f ||
        static_cast<int>(respawnInvincibilityTimer * 10) % 2 == 0) {
        player.Render(renderer);
    }
}

void Game::RenderUI() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

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
    SDL_FRect waveBar{600.0f, 10.0f, static_cast<float>(wave * 30), 16.0f};
    SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
    SDL_RenderFillRect(renderer, &waveBar);

    // Bomb cooldown bar (top-right). Bright yellow when fully charged.
    {
        const float barW = 110.0f;
        const float barH = 16.0f;
        const float barX = static_cast<float>(windowWidth) - barW - 20.0f;
        const float barY = 40.0f;

        SDL_FRect background{barX, barY, barW, barH};
        SDL_SetRenderDrawColor(renderer, 25, 25, 45, 255);
        SDL_RenderFillRect(renderer, &background);

        const float charge = std::min(1.0f, bombCooldown / kBombCooldownTime);
        SDL_FRect fill{barX + 2.0f, barY + 2.0f,
                       (barW - 4.0f) * charge, barH - 4.0f};
        if (charge >= 1.0f) {
            SDL_SetRenderDrawColor(renderer, 255, 220, 80, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);
        }
        SDL_RenderFillRect(renderer, &fill);
    }

    // Game Over screen
    if (state == GameState::GameOver) {
        // Semi-transparent overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_FRect overlay{0.0f, 0.0f,
                          static_cast<float>(windowWidth),
                          static_cast<float>(windowHeight)};
        SDL_RenderFillRect(renderer, &overlay);

        // Game Over box, centred in the window
        const float boxW = 400.0f;
        const float boxH = 300.0f;
        const SDL_FRect box{(static_cast<float>(windowWidth) - boxW) * 0.5f,
                            (static_cast<float>(windowHeight) - boxH) * 0.5f,
                            boxW, boxH};
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        SDL_RenderFillRect(renderer, &box);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &box);

        if (nameEntryActive) {
            // Name entry
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect nameBox{box.x + 50.0f, box.y + 100.0f, 300.0f, 40.0f};
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
