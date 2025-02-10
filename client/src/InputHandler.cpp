#include "InputHandler.hpp"
#include "Physics.hpp"

InputHandler::PlayerInputs InputHandler::handleInput(Player *player, std::vector<std::unique_ptr<Button>> &buttonList)
{
    SDL_Event event;
    quit = false;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            quit = true;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHT:
                player->setRotationSpeed(0.4);
                break;
            case SDLK_LEFT:
                player->setRotationSpeed(-0.4);
                break;
            case SDLK_SPACE:
                player->setThrust(10000);
                break;
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHT:
                player->setRotationSpeed(0);
                break;
            case SDLK_LEFT:
                player->setRotationSpeed(0);
                break;
            case SDLK_SPACE:
                player->setThrust(0);
                break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            for (std::unique_ptr<Button> &button : buttonList)
            {
                button->handleEvent(event);
            }
        }
    }

    playerInputs.thrust = player->getThrust();
    playerInputs.rotation = player->getRotation();

    return playerInputs; 
}
