#include "InputHandler.hpp"

void InputHandler::handleInput(Player* player) {
    SDL_Event event;
    quit = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_RIGHT:
                
                break;
            case SDLK_LEFT:
                
                break;
            case SDLK_SPACE:
                player->setThrust(10000);
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_RIGHT:
                
            case SDLK_LEFT:
                
                break;
            case SDLK_SPACE:
                player->setThrust(0);
                break;
        }
    }
    }
}
