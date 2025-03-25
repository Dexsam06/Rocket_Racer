#include "../include/InputHandler.hpp"

void InputHandler::handleInput(std::vector<std::unique_ptr<Button>> &buttonList)
{
    SDL_Event event;
    uint32_t currentTime;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.repeat == 0) 
            {
                currentTime = SDL_GetTicks();
                sendInputToServer(ClientInputPacket(event.key.keysym.sym, true)); 
            }
            break;

        case SDL_KEYUP:
                currentTime = SDL_GetTicks();
                sendInputToServer(ClientInputPacket(event.key.keysym.sym, false));  
            break;

        case SDL_QUIT:
            quit = true;
            break;

        case SDL_MOUSEBUTTONDOWN:
            for (std::unique_ptr<Button> &button : buttonList)
            {
                button->handleEvent(event);
            }
            break;
        }
    }
}
