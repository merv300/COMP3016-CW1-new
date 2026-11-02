// 1. Include the main SDL header
#include <SDL3/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // 2. Initialize the SDL video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 3. Create a window and a renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("Hello SDL3", 800, 600, 0, &window, &renderer)) {
        std::cerr << "Window/Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // 4. The main loop
    bool running = true;
    while (running) {
        // 4a. Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // 4b. Render a frame
        // Set the draw colour to a nice blue
        SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
        // Clear the screen with the draw colour
        SDL_RenderClear(renderer);
        // Present the rendered frame to the screen
        SDL_RenderPresent(renderer);
    }

    // 5. Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}