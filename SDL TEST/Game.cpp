#include "Game.h"

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

    if (input.WasKeyPressed(SDL_SCANCODE_Q)) {
        quarterTurns = (quarterTurns + 1) % 4;
    }

    // Input is captured in screen space, then remapped into world space so the
    // player always moves the way the key points on the rotated screen.
    const Vec2 screenDirection = input.GetMoveAxis();
    player.SetMoveIntent(WorldFromScreen(screenDirection, quarterTurns));
    player.SetFacingQuarterTurns(quarterTurns);
    player.SetFiring(input.IsFiring());
    player.Update(deltaTime);
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
    const double angle = 90.0 * static_cast<double>(quarterTurns);

    SDL_RenderTextureRotated(renderer, worldTexture, nullptr, &destination, angle, &center, SDL_FLIP_NONE);

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

    player.Render(renderer);
}

} // namespace rtz