#include "InputHandler.hpp"

void InputHandler::handleInput() {
    SDL_Event event;
    quit = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
    }
}
