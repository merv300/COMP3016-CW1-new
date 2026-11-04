#include <exception>
#include <iostream>

#include "Game.h"

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        rtz::Game game;
        game.Run();
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
