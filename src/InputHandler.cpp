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
                player->setRotationSpeed((0.01 * 180) / M_PI); 
                break;
            case SDLK_LEFT: 
                player->setRotationSpeed(-((0.01 * 180) / M_PI)); 
                break;
            case SDLK_SPACE:
                player->setThrust(20000);
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_RIGHT:
                player->setRotationSpeed(0); 
            case SDLK_LEFT:
                player->setRotationSpeed(0); 
                break;
            case SDLK_SPACE:
                player->setThrust(0);
                break;
        }
    }
    }
}
