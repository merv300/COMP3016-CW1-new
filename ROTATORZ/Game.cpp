#define _CRT_SECURE_NO_WARNINGS
#include "Game.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
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

// 5x7 bitmap digits for rendering scores without SDL3_ttf.
// Each row is a bitmask where bit 4 (leftmost) is the first column.
constexpr uint8_t kDigitFont[10][7] = {
    { 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111 }, // 0
    { 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 }, // 1
    { 0b11111, 0b00001, 0b00001, 0b11111, 0b10000, 0b10000, 0b11111 }, // 2
    { 0b11111, 0b00001, 0b00001, 0b01111, 0b00001, 0b00001, 0b11111 }, // 3
    { 0b10001, 0b10001, 0b10001, 0b11111, 0b00001, 0b00001, 0b00001 }, // 4
    { 0b11111, 0b10000, 0b10000, 0b11111, 0b00001, 0b00001, 0b11111 }, // 5
    { 0b11111, 0b10000, 0b10000, 0b11111, 0b10001, 0b10001, 0b11111 }, // 6
    { 0b11111, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001 }, // 7
    { 0b11111, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b11111 }, // 8
    { 0b11111, 0b10001, 0b10001, 0b11111, 0b00001, 0b00001, 0b11111 }, // 9
};

// 5x7 bitmap letters for rendering text labels without SDL3_ttf.
// Index: 'A' = 0 ... 'Z' = 25, space = 26.
constexpr uint8_t kLetterFont[27][7] = {
    { 0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 }, // A
    { 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110 }, // B
    { 0b01110, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01110 }, // C
    { 0b11100, 0b10010, 0b10001, 0b10001, 0b10001, 0b10010, 0b11100 }, // D
    { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 }, // E
    { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000 }, // F
    { 0b01110, 0b10000, 0b10000, 0b10111, 0b10001, 0b10001, 0b01110 }, // G
    { 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 }, // H
    { 0b01111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01111 }, // I
    { 0b00011, 0b00010, 0b00010, 0b00010, 0b00010, 0b10010, 0b01100 }, // J
    { 0b10001, 0b10011, 0b10101, 0b11001, 0b10101, 0b10011, 0b10001 }, // K
    { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111 }, // L
    { 0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001 }, // M
    { 0b10001, 0b11001, 0b10101, 0b10101, 0b10011, 0b10001, 0b10001 }, // N
    { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 }, // O
    { 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000 }, // P
    { 0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b11001, 0b01111 }, // Q
    { 0b11110, 0b10001, 0b10001, 0b11110, 0b10101, 0b10011, 0b10001 }, // R
    { 0b01110, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b01110 }, // S
    { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 }, // T
    { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 }, // U
    { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100 }, // V
    { 0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b01010, 0b00100 }, // W
    { 0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001 }, // X
    { 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100 }, // Y
    { 0b11111, 0b00001, 0b00011, 0b00101, 0b01001, 0b10001, 0b11111 }, // Z
    { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 }, // space (26)
};

// Draws an integer as bitmap digits (no SDL3_ttf dependency).
// Returns the pixel width of the rendered number.
int DrawNumber(SDL_Renderer* renderer, int x, int y, int value, int scale = 2) {
    if (value < 0) {
        value = 0;
    }

    // Extract digits (least significant first).
    int digits[10] = {0};
    int digitCount = 1;
    int temp = value;
    while (temp >= 10) {
        digits[digitCount - 1] = temp % 10;
        temp /= 10;
        ++digitCount;
    }
    digits[digitCount - 1] = temp;

    // Draw most-significant digit first.
    int drawX = x - digitCount * 5 * scale;
    for (int d = digitCount - 1; d >= 0; --d) {
        uint8_t digit = static_cast<uint8_t>(digits[d]);
        for (int row = 0; row < 7; ++row) {
            uint8_t rowBits = kDigitFont[digit][row];
            for (int col = 0; col < 5; ++col) {
                if ((rowBits >> (4 - col)) & 1u) {
                    SDL_FRect pixel{
                        static_cast<float>(drawX + col * scale),
                        static_cast<float>(y + row * scale),
                        static_cast<float>(scale),
                        static_cast<float>(scale)
                    };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        drawX += 5 * scale + scale; // digit width + 1px gap
    }

    return digitCount * 5 * scale + (digitCount - 1) * scale;
}

// Draws a text string as bitmap letters (no SDL3_ttf dependency).
// Only uppercase A-Z and space are supported. Returns the pixel width.
int DrawText(SDL_Renderer* renderer, int x, int y, const char* text, int scale = 1) {
    int totalWidth = 0;
    int drawX = x;

    for (const char* p = text; *p != '\0'; ++p) {
        char c = *p;
        if (c == ' ') {
            drawX += 5 * scale + scale;
            totalWidth += 5 * scale + scale;
            continue;
        }

        int letterIndex = c - 'A';
        if (letterIndex < 0 || letterIndex > 25) {
            continue;
        }

        for (int row = 0; row < 7; ++row) {
            uint8_t rowBits = kLetterFont[letterIndex][row];
            for (int col = 0; col < 5; ++col) {
                if ((rowBits >> (4 - col)) & 1u) {
                    SDL_FRect pixel{
                        static_cast<float>(drawX + col * scale),
                        static_cast<float>(y + row * scale),
                        static_cast<float>(scale),
                        static_cast<float>(scale)
                    };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        drawX += 5 * scale + scale;
        totalWidth += 5 * scale + scale;
    }

    return totalWidth;
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
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

#ifdef RTZ_USE_SDL3_MIXER
    // SDL3_mixer for music and sound effects. Audio is optional: if the
    // device cannot be opened the game still runs, just silently.
    if (MIX_Init()) {
        // Create a mixer attached to the default playback device.
        // MIX_CreateMixer() alone only renders into a memory buffer and stays
        // silent, so MIX_CreateMixerDevice() is required for real audio output.
        SDL_AudioSpec spec{};
        spec.freq = 48000;
        spec.format = SDL_AUDIO_F32;
        spec.channels = 2;
        mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
        if (mixer == nullptr) {
            SDL_Log("Audio disabled, no playback device: %s", SDL_GetError());
        }
    } else {
        SDL_Log("MIX_Init failed, running without audio: %s", SDL_GetError());
    }
#endif

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

    // Load sprites, music and SFX.
    LoadAssets();

    ResetGame();
    LoadHighScores();
}

void Game::Shutdown() {
    UnloadAssets();

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
#ifdef RTZ_USE_SDL3_MIXER
    if (mixer != nullptr) {
        MIX_DestroyMixer(mixer);
        mixer = nullptr;
    }
    MIX_Quit();
#endif
    SDL_Quit();
}

void Game::LoadAssets() {
    // Sprites — falls back to coloured rectangles if files are missing.
    Player::LoadTexture(renderer, kAssetPlayerSprite);
    Enemy::LoadTextures(renderer, kAssetEnemyPortrait, kAssetEnemyLandscape);

#ifdef RTZ_USE_SDL3_MIXER
    if (mixer == nullptr) {
        return;  // Audio device unavailable; sprites still work.
    }

    // Music — stream from disk and loop forever.
    musicAudio = MIX_LoadAudio(mixer, kAssetMusic, false);
    if (musicAudio != nullptr) {
        musicTrack = MIX_CreateTrack(mixer);
        MIX_SetTrackAudio(musicTrack, musicAudio);

        // The loop count has to be supplied when playback starts; setting it
        // on a stopped track is overwritten by MIX_PlayTrack().
        SDL_PropertiesID playProps = SDL_CreateProperties();
        SDL_SetNumberProperty(playProps, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        MIX_PlayTrack(musicTrack, playProps);
        SDL_DestroyProperties(playProps);
    } else {
        SDL_Log("Music failed to load: %s", SDL_GetError());
    }

    // Sound effects — predecoded so they can retrigger instantly.
    shootAudio = MIX_LoadAudio(mixer, kAssetShootSfx, true);
    if (shootAudio == nullptr) {
        SDL_Log("Shoot SFX failed to load: %s", SDL_GetError());
    }

    // Played when a shot hits an enemy aligned to the other axis.
    wrongHitAudio = MIX_LoadAudio(mixer, kAssetWrongHitSfx, true);
    if (wrongHitAudio == nullptr) {
        SDL_Log("Wrong-orientation hit SFX failed to load: %s", SDL_GetError());
    }

    // Enemy shots: several voices at a reduced gain so the frequent fire does
    // not drown out the music and player feedback.
    enemyShootAudio = MIX_LoadAudio(mixer, kAssetEnemyShootSfx, true);
    if (enemyShootAudio != nullptr) {
        for (int i = 0; i < kEnemyShootVoices; ++i) {
            enemyShootTracks[i] = MIX_CreateTrack(mixer);
            if (enemyShootTracks[i] != nullptr) {
                MIX_SetTrackAudio(enemyShootTracks[i], enemyShootAudio);
                MIX_SetTrackGain(enemyShootTracks[i], kEnemyShootGain);
            }
        }
    } else {
        SDL_Log("Enemy shoot SFX failed to load: %s", SDL_GetError());
    }
#endif
}

void Game::UnloadAssets() {
#ifdef RTZ_USE_SDL3_MIXER
    if (shootAudio != nullptr) {
        MIX_DestroyAudio(shootAudio);
        shootAudio = nullptr;
    }
    if (wrongHitAudio != nullptr) {
        MIX_DestroyAudio(wrongHitAudio);
        wrongHitAudio = nullptr;
    }
    for (int i = 0; i < kEnemyShootVoices; ++i) {
        if (enemyShootTracks[i] != nullptr) {
            MIX_DestroyTrack(enemyShootTracks[i]);
            enemyShootTracks[i] = nullptr;
        }
    }
    if (enemyShootAudio != nullptr) {
        MIX_DestroyAudio(enemyShootAudio);
        enemyShootAudio = nullptr;
    }
    if (musicTrack != nullptr) {
        MIX_DestroyTrack(musicTrack);
        musicTrack = nullptr;
    }
    if (musicAudio != nullptr) {
        MIX_DestroyAudio(musicAudio);
        musicAudio = nullptr;
    }
#endif
    Player::UnloadTexture();
    Enemy::UnloadTextures();
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

    if (state == GameState::Menu) {
        UpdateMenu(deltaTime);
    } else if (state == GameState::Playing) {
        UpdatePlaying(deltaTime);
    } else if (state == GameState::Leaderboard) {
        UpdateLeaderboard(deltaTime);
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
    if (wrongHitSfxCooldown > 0.0f) {
        wrongHitSfxCooldown -= deltaTime;
    }
    if (input.IsFiring() && fireCooldown <= 0.0f) {
        fireCooldown = kFireCooldown;
        const Vec2 forward{0.0f, -1.0f};
        const Vec2 spawnPos = player.GetPosition() + forward * (kPlayerSize * 0.6f);

        // Capture the view rotation at fire time so a bullet's killable
        // orientation stays fixed even if the player rotates mid-flight.
        const int firedQt = (targetAngle == 90.0f) ? 1 : 0;
        bulletPool.Acquire(spawnPos, forward, kPlayerBulletSpeed, BulletType::Player, firedQt);

        // Shoot sound effect (fire-and-forget; SDL_mixer manages the track).
#ifdef RTZ_USE_SDL3_MIXER
        if (mixer != nullptr && shootAudio != nullptr) {
            MIX_PlayAudio(mixer, shootAudio);
        }
#endif
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

    // Stars drift downward and wrap around.
    starPool.ForEachActive([deltaTime](Star& star) { star.Update(deltaTime); });

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
        nameEntryActive = false;
        nameLength = 0;
        nameBuffer[0] = '\0';
        gameOverMenu = GameOverMenu::Retry;
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
            PlayEnemyShootSound();
        }
    });
}

void Game::PlayEnemyShootSound() {
#ifdef RTZ_USE_SDL3_MIXER
    if (mixer == nullptr || enemyShootAudio == nullptr) {
        return;
    }

    // Rotate through the voices so overlapping shots all get heard.
    MIX_Track* track = enemyShootTracks[enemyShootVoice];
    enemyShootVoice = (enemyShootVoice + 1) % kEnemyShootVoices;
    if (track != nullptr) {
        MIX_PlayTrack(track, 0);
    }
#endif
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
            AddHighScore(nameBuffer, score);
            SaveHighScores();
            nameEntryActive = false;
            state = GameState::Leaderboard;
        }
        return;
    }

    // Menu navigation: Up/Down or W/S to switch between Retry and Enter Name.
    if (input.WasKeyPressed(SDL_SCANCODE_UP) ||
        input.WasKeyPressed(SDL_SCANCODE_DOWN) ||
        input.WasKeyPressed(SDL_SCANCODE_W) ||
        input.WasKeyPressed(SDL_SCANCODE_S)) {
        gameOverMenu = (gameOverMenu == GameOverMenu::Retry)
            ? GameOverMenu::EnterName
            : GameOverMenu::Retry;
    }

    // Select the highlighted option.
    if (input.WasKeyPressed(SDL_SCANCODE_SPACE) ||
        input.WasKeyPressed(SDL_SCANCODE_RETURN) ||
        input.WasKeyPressed(SDL_SCANCODE_KP_ENTER)) {
        if (gameOverMenu == GameOverMenu::Retry) {
            ResetGame();
            state = GameState::Playing;
        } else {
            nameEntryActive = true;
            nameLength = 0;
            nameBuffer[0] = '\0';
        }
    }
}

void Game::LoadHighScores() {
    FILE* file = std::fopen(kScoresFilename, "rb");
    if (file == nullptr) {
        // No save file yet — start with an empty table.
        highScoreCount = 0;
        return;
    }

    // Read the entry count, then each entry.
    int count = 0;
    if (std::fread(&count, sizeof(count), 1, file) != 1) {
        count = 0;
    }
    if (count < 0 || count > kMaxHighScores) {
        count = 0;
    }

    for (int i = 0; i < count; ++i) {
        // Bounds check: never read past the table size (fixes C6385/C6386).
        if (i >= kMaxHighScores) {
            break;
        }
        if (std::fread(&highScores[i], sizeof(HighScoreEntry), 1, file) != 1) {
            break;
        }
        highScoreCount = i + 1;
    }

    std::fclose(file);
}

void Game::SaveHighScores() {
    FILE* file = std::fopen(kScoresFilename, "wb");
    if (file == nullptr) {
        return;
    }

    std::fwrite(&highScoreCount, sizeof(highScoreCount), 1, file);
    for (int i = 0; i < highScoreCount; ++i) {
        // Bounds check: never write past the table size (fixes C6385/C6386).
        if (i >= kMaxHighScores) {
            break;
        }
        std::fwrite(&highScores[i], sizeof(HighScoreEntry), 1, file);
    }

    std::fclose(file);
}

void Game::AddHighScore(const char* name, int scoreValue) {
    // Find the insertion point: keep the table sorted descending by score.
    int insertAt = highScoreCount;
    while (insertAt > 0 && insertAt <= kMaxHighScores &&
           highScores[insertAt - 1].score < scoreValue) {
        --insertAt;
    }

    // Bounds check: never write outside the high score table (fixes C6385/C6386).
    if (insertAt < 0 || insertAt >= kMaxHighScores) {
        return;
    }

    // If the table is full and the new score doesn't beat the worst entry, ignore.
    if (highScoreCount >= kMaxHighScores &&
        highScores[kMaxHighScores - 1].score >= scoreValue) {
        return;
    }

    // Shift entries down to make room.
    int end = highScoreCount;
    if (end < kMaxHighScores) {
        ++end;
    }
    // Bounds check: clamp end to table size.
    if (end > kMaxHighScores) {
        end = kMaxHighScores;
    }
    for (int i = end - 1; i > insertAt; --i) {
        // Bounds check: only shift valid indices (fixes C6385/C6386).
        if (i >= 0 && i < kMaxHighScores && i - 1 >= 0 && i - 1 < kMaxHighScores) {
            highScores[i] = highScores[i - 1];
        }
    }

    // Insert the new entry.
    // Bounds check: analyzer may not track insertAt across the shift loop,
    // so re-validate before writing (fixes C6386).
    if (insertAt < 0 || insertAt >= kMaxHighScores) {
        return;
    }

    HighScoreEntry entry = {};
    std::strncpy(entry.name, name, sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = '\0';
    entry.score = scoreValue;
    highScores[insertAt] = entry;

    if (highScoreCount < kMaxHighScores) {
        ++highScoreCount;
    }
}

void Game::UpdateLeaderboard(float deltaTime) {
    (void)deltaTime;

    if (input.WasKeyPressed(SDL_SCANCODE_SPACE) ||
        input.WasKeyPressed(SDL_SCANCODE_RETURN) ||
        input.WasKeyPressed(SDL_SCANCODE_KP_ENTER) ||
        input.WasKeyPressed(SDL_SCANCODE_ESCAPE)) {
        ResetGame();
        state = GameState::Menu;
    }
}

void Game::UpdateMenu(float deltaTime) {
    // Animate starfield background
    starPool.ForEachActive([deltaTime](Star& star) { star.Update(deltaTime); });

    // Menu navigation: Up/W moves up, Down/S moves down (wrap around).
    const int optionCount = 3;
    if (input.WasKeyPressed(SDL_SCANCODE_UP) ||
        input.WasKeyPressed(SDL_SCANCODE_W)) {
        menuSelected = (menuSelected - 1 + optionCount) % optionCount;
    } else if (input.WasKeyPressed(SDL_SCANCODE_DOWN) ||
               input.WasKeyPressed(SDL_SCANCODE_S)) {
        menuSelected = (menuSelected + 1) % optionCount;
    }

    // Select the highlighted option.
    if (input.WasKeyPressed(SDL_SCANCODE_SPACE) ||
        input.WasKeyPressed(SDL_SCANCODE_RETURN) ||
        input.WasKeyPressed(SDL_SCANCODE_KP_ENTER)) {
        if (menuSelected == 0) {
            // Start game
            ResetGame();
            state = GameState::Playing;
        } else if (menuSelected == 1) {
            // Leaderboard
            state = GameState::Leaderboard;
        } else if (menuSelected == 2) {
            // Exit
            running = false;
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
    // Player bullets vs enemies (orientation-gated).
    bulletPool.ForEachActive([this](Bullet& bullet) {
        if (!bullet.IsActive()) {
            return;
        }

        // The quarter-turn in effect when this bullet was fired. This stays
        // fixed for the bullet's lifetime, so rotating the view mid-flight
        // cannot change what an in-flight bullet is allowed to kill.
        const int bulletQt = bullet.GetFiredQuarterTurns();

        enemyPool.ForEachActive([this, &bullet, bulletQt](Enemy& enemy) {
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
            // actually destroys the enemy.
            bullet.Deactivate();

            if (enemy.IsVulnerableAt(bulletQt)) {
                enemy.Deactivate();
                score += enemy.GetScoreValue();
            } else {
                // Wrong axis: the shot is absorbed but the enemy survives.
                // Play a distinct "clang" so the player learns to rotate.
#ifdef RTZ_USE_SDL3_MIXER
                if (mixer != nullptr && wrongHitAudio != nullptr &&
                    wrongHitSfxCooldown <= 0.0f) {
                    MIX_PlayAudio(mixer, wrongHitAudio);
                    wrongHitSfxCooldown = kWrongHitSfxCooldown;
                }
#endif
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
    wrongHitSfxCooldown = 0.0f;
    bombCooldown = kBombCooldownTime;
    respawnInvincibilityTimer = 0.0f;
    currentAngle = 0.0f;
    targetAngle = 0.0f;
    nameEntryActive = false;
    nameLength = 0;
    nameBuffer[0] = '\0';
    gameOverMenu = GameOverMenu::Retry;

    bulletPool.ReleaseAll();
    enemyBulletPool.ReleaseAll();
    enemyPool.ReleaseAll();
    bombPool.ReleaseAll();
    explosionPool.ReleaseAll();

    player.SetPosition({kWorldWidth * 0.5f, kPlayerY});
    player.SetMoveIntent(0.0f);
    player.SetFiring(false);
    player.SetViewAngle(0.0f);

    // Initialise the starfield background.
    starPool.ReleaseAll();
    for (int i = 0; i < kStarCount; ++i) {
        const int layer = std::rand() % kStarLayers;
        const float size = 1.0f + layer * 0.8f;
        const float x = static_cast<float>(std::rand() % static_cast<int>(kWorldWidth));
        const float y = static_cast<float>(std::rand() % static_cast<int>(kWorldHeight));
        const float speed = 15.0f + layer * 40.0f;
        const Uint8 alpha = static_cast<Uint8>(90 + layer * 65);
        starPool.Acquire(x, y, size, speed, alpha);
    }

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
    if (state == GameState::Menu) {
        RenderMenu(windowWidth, windowHeight);
    } else {
        RenderUI();
    }

    SDL_RenderPresent(renderer);
}

void Game::DrawWorld() {
    SDL_SetRenderDrawColor(renderer, 10, 12, 24, 255);
    SDL_RenderClear(renderer);

    // Starfield background
    starPool.ForEachActive([this](Star& star) { star.Render(renderer); });

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

    // Score — bitmap digits in top-right (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const int scoreTextWidth = DrawNumber(renderer, windowWidth - 20, 10, score, 4);

    // Lives
    for (int i = 0; i < lives; ++i) {
        SDL_FRect lifeIcon{10.0f + i * 28.0f, 40.0f, 20.0f, 20.0f};
        SDL_SetRenderDrawColor(renderer, 120, 220, 255, 255);
        SDL_RenderFillRect(renderer, &lifeIcon);
    }

    // Wave
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
            // Name entry field
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect nameBox{box.x + 50.0f, box.y + 100.0f, 300.0f, 40.0f};
            SDL_RenderRect(renderer, &nameBox);

            // Draw the entered name (scale 2 for legibility)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            DrawText(renderer, static_cast<int>(box.x + 60.0f),
                     static_cast<int>(box.y + 108.0f), nameBuffer, 2);

            // Blinking cursor after the name
            if (static_cast<int>(SDL_GetTicks() / 500) % 2 == 0) {
                int nameWidth = 0;
                for (int i = 0; nameBuffer[i] != '\0'; ++i) {
                    nameWidth += 5 * 2 + 1 * 2; // char width + gap at scale 2
                }
                SDL_FRect cursor{static_cast<float>(box.x + 60.0f + nameWidth),
                                 static_cast<float>(box.y + 108.0f),
                                 2.0f * 2.0f, 7.0f * 2.0f};
                SDL_RenderFillRect(renderer, &cursor);
            }
        } else {
            // Title
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
            DrawNumber(renderer, static_cast<int>(box.x + boxW * 0.5f),
                       static_cast<int>(box.y + 50.0f), score, 5);

            // Menu options
            const float optionY = box.y + 150.0f;
            const float optionGap = 44.0f;

            // Retry
            {
                const bool selected = (gameOverMenu == GameOverMenu::Retry);
                const Uint8 r = selected ? 255 : 120;
                const Uint8 g = selected ? 220 : 120;
                const Uint8 b = selected ? 80 : 120;
                const Uint8 a = selected ? 255 : 150;

                const float iconX = box.x + boxW * 0.5f - 60.0f;

                // Selection arrow
                if (selected) {
                    SDL_SetRenderDrawColor(renderer, r, g, b, a);
                    SDL_FRect arrow{iconX, static_cast<float>(optionY + 6.0f), 14.0f, 14.0f};
                    SDL_RenderFillRect(renderer, &arrow);
                }

                // Digit icon (0 for Retry)
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                DrawNumber(renderer, static_cast<int>(iconX + 20.0f), static_cast<int>(optionY), 0, 3);

                // Text label below as translucent hint
                SDL_SetRenderDrawColor(renderer, r / 2, g / 2, b / 2, a / 2);
                DrawText(renderer, static_cast<int>(iconX + 40.0f),
                         static_cast<int>(optionY + 24.0f), "RETRY", 2);
            }

            // Enter Name
            {
                const bool selected = (gameOverMenu == GameOverMenu::EnterName);
                const Uint8 r = selected ? 255 : 120;
                const Uint8 g = selected ? 220 : 120;
                const Uint8 b = selected ? 80 : 120;
                const Uint8 a = selected ? 255 : 150;

                const float iconX = box.x + boxW * 0.5f - 60.0f;

                // Selection arrow
                if (selected) {
                    SDL_SetRenderDrawColor(renderer, r, g, b, a);
                    SDL_FRect arrow{iconX, static_cast<float>(optionY + optionGap + 6.0f), 14.0f, 14.0f};
                    SDL_RenderFillRect(renderer, &arrow);
                }

                // Digit icon (1 for Enter Name)
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                DrawNumber(renderer, static_cast<int>(iconX + 20.0f), static_cast<int>(optionY + optionGap), 1, 3);

                // Text label below as translucent hint
                SDL_SetRenderDrawColor(renderer, r / 2, g / 2, b / 2, a / 2);
                DrawText(renderer, static_cast<int>(iconX + 40.0f),
                         static_cast<int>(optionY + optionGap + 24.0f), "ENTER NAME", 2);
            }

            // Hint
            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
            SDL_FRect hintBar{box.x + 40.0f, box.y + boxH - 40.0f, 320.0f, 16.0f};
            SDL_RenderFillRect(renderer, &hintBar);
        }
    }

    // Leaderboard screen
    if (state == GameState::Leaderboard) {
        RenderLeaderboard(windowWidth, windowHeight);
    }
}

void Game::RenderLeaderboard(int windowWidth, int windowHeight) {
    // Opaque overlay so the game world does not show through behind the box
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_FRect overlay{0.0f, 0.0f,
                      static_cast<float>(windowWidth),
                      static_cast<float>(windowHeight)};
    SDL_RenderFillRect(renderer, &overlay);

    // Leaderboard box, centred in the window
    const float boxW = 450.0f;
    const float boxH = 500.0f;
    const SDL_FRect box{(static_cast<float>(windowWidth) - boxW) * 0.5f,
                        (static_cast<float>(windowHeight) - boxH) * 0.5f,
                        boxW, boxH};
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &box);

    // Title bar
    SDL_SetRenderDrawColor(renderer, 80, 200, 120, 255);
    SDL_FRect titleBar{box.x + 20.0f, box.y + 25.0f, 200.0f, 24.0f};
    SDL_RenderFillRect(renderer, &titleBar);

    // Entries (text labels — legible without SDL3_ttf)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const float colXName = box.x + 70.0f;
    const float colXScore = box.x + 300.0f;

    // Column headers (bright, larger)
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    DrawText(renderer, static_cast<int>(colXName), static_cast<int>(box.y + 62.0f), "NAME", 3);
    DrawText(renderer, static_cast<int>(colXScore), static_cast<int>(box.y + 62.0f), "SCORE", 3);

    // Calculate how many entries fit in the box (leave room for hint bar at bottom)
    const float entryStartY = box.y + 102.0f;
    const float entrySpacing = 72.0f;
    const int maxEntriesVisible = static_cast<int>((boxH - 102.0f - 60.0f) / entrySpacing);
    int entriesToShow = highScoreCount;
    if (entriesToShow > kMaxHighScores) entriesToShow = kMaxHighScores;
    if (entriesToShow > maxEntriesVisible) entriesToShow = maxEntriesVisible;

    for (int i = 0; i < entriesToShow; ++i) {
        // Bounds check: only read valid entries (fixes C6385/C6386).
        if (i < 0 || i >= kMaxHighScores || i >= highScoreCount) {
            break;
        }

        const float entryY = entryStartY + i * entrySpacing;

        // Rank indicator
        SDL_FRect rankBar{box.x + 40.0f, entryY, 24.0f, 24.0f};
        if (i == 0) {
            SDL_SetRenderDrawColor(renderer, 255, 220, 80, 255);   // gold
        } else if (i == 1) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);  // silver
        } else if (i == 2) {
            SDL_SetRenderDrawColor(renderer, 205, 127, 50, 255);   // bronze
        } else {
            SDL_SetRenderDrawColor(renderer, 120, 220, 255, 255);  // blue
        }
        SDL_RenderFillRect(renderer, &rankBar);

        // Rank number (scale 2)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        DrawNumber(renderer, static_cast<int>(box.x + 52.0f), static_cast<int>(entryY + 2.0f), i + 1, 2);

        // Name text (scale 3 for legibility)
        const char* entryName = highScores[i].name[0] != '\0' ? highScores[i].name : "--";
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        DrawText(renderer, static_cast<int>(colXName), static_cast<int>(entryY), entryName, 3);

        // Score text (scale 3 for legibility)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        DrawNumber(renderer, static_cast<int>(colXScore), static_cast<int>(entryY), highScores[i].score, 3);
    }

    // Hint bar at bottom, below all entries
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_FRect hintBar{box.x + 40.0f, box.y + boxH - 48.0f, 200.0f, 16.0f};
    SDL_RenderFillRect(renderer, &hintBar);
}

void Game::RenderMenu(int windowWidth, int windowHeight) {
    // Draw world background (stars) — cleared by DrawWorld already, but we
    // need to draw stars directly since we skip DrawWorld for the menu.
    SDL_SetRenderDrawColor(renderer, 10, 12, 24, 255);
    SDL_RenderClear(renderer);

    // Draw stars directly for animated background. Scale both axes so the
    // starfield fills the window correctly at any resolution / in fullscreen.
    const float starScaleX = static_cast<float>(windowWidth) / kWorldWidth;
    const float starScaleY = static_cast<float>(windowHeight) / kWorldHeight;
    starPool.ForEachActive([this, starScaleX, starScaleY](Star& star) {
        star.Render(renderer, starScaleX, starScaleY);
    });

    // Menu box, centred in the window
    const float boxW = 400.0f;
    const float boxH = 300.0f;
    const SDL_FRect box{(static_cast<float>(windowWidth) - boxW) * 0.5f,
                        (static_cast<float>(windowHeight) - boxH) * 0.5f,
                        boxW, boxH};
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &box);

    // Title "ROTATORZ" — large bitmap letters at scale 5, centred in the box
    const int titleScale = 5;
    const char* title = "ROTATORZ";
    const int titleWidth =
        static_cast<int>(std::strlen(title)) * 6 * titleScale - titleScale;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    DrawText(renderer, static_cast<int>(box.x + (boxW - titleWidth) * 0.5f),
             static_cast<int>(box.y + 50.0f), title, titleScale);

    // Menu options
    // Menu options
    float optionY = box.y + 140.0f;
    const float optionGap = 44.0f;

    const char* options[3] = {"START GAME", "LEADERBOARD", "EXIT"};

    for (int i = 0; i < 3; ++i) {
        const bool selected = (menuSelected == i);
        const Uint8 r = selected ? 255 : 120;
        const Uint8 g = selected ? 220 : 120;
        const Uint8 b = selected ? 80 : 120;
        const Uint8 a = selected ? 255 : 150;

        // Selection arrow on the left
        if (selected) {
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_FRect arrow{box.x + 60.0f, static_cast<float>(optionY + 8.0f), 14.0f, 14.0f};
            SDL_RenderFillRect(renderer, &arrow);
        }

        // Option text
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        DrawText(renderer, static_cast<int>(box.x + 90.0f),
                 static_cast<int>(optionY), options[i], 2);

        optionY += optionGap;
    }

    // Hint at bottom
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_FRect hintBar{box.x + 40.0f, box.y + boxH - 40.0f, 320.0f, 16.0f};
    SDL_RenderFillRect(renderer, &hintBar);
}

} // namespace rtz
