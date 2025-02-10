#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>
#include "Player.hpp"
#include "Button.hpp"

class InputHandler {
public: 
    struct PlayerInputs
    {
        double thrust;
        double rotation;
    }; 

    PlayerInputs handleInput(Player* player, std::vector<std::unique_ptr<Button>> &buttonList);
    bool isQuit() const { return quit; } 

private:
    bool quit = false;
    PlayerInputs playerInputs;
};

#endif 
