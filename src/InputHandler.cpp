#include "InputHandler.hpp"
#include "Physics.hpp"

void InputHandler::handleInput(Player* player, std::vector<Button*> buttonList) { 
    SDL_Event event; 
    quit = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_RIGHT:
                player->setRotationSpeed(0.3); 
                break;
            case SDLK_LEFT: 
                player->setRotationSpeed(-0.3); 
                break;
            case SDLK_SPACE:
                player->setThrust(10000);
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
      else if (event.type == SDL_MOUSEBUTTONDOWN) { 
        for(Button* button : buttonList) {
            button->handleEvent(event);
        }
      }
    }
}
