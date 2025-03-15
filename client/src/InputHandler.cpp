#include "../include/InputHandler.hpp"

std::vector<KeyInput> InputHandler::handleInput(std::vector<std::unique_ptr<Button>> &buttonList)
{
    SDL_Event event;
    std::vector<KeyInput> keyInputPacket;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.repeat == 0) 
            {
                if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT ||
                    event.key.keysym.sym == SDLK_SPACE)
                {
                    keyPressTimes[event.key.keysym.sym] = SDL_GetTicks(); 
                }
            }
            break;

        case SDL_KEYUP:
            if (keyPressTimes.count(event.key.keysym.sym))
            {
                Uint32 pressTime = keyPressTimes[event.key.keysym.sym];
                Uint32 releaseTime = SDL_GetTicks();
                float duration = (releaseTime - pressTime) / 1000.0f; // Convert to seconds

                keyHoldDurations[event.key.keysym.sym] = duration;
                keyPressTimes.erase(event.key.keysym.sym); // Remove recorded press time
            }
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

    if (!keyHoldDurations.empty()) {
        for (const auto &key : keyHoldDurations) 
        {
            int keyType = key.first; 
            float duration = key.second;

            keyInputPacket.push_back({keyType, duration});
        } 

        // Clear keyHoldDurations after sending the data
        keyHoldDurations.clear();
    }

    return keyInputPacket;  
}
