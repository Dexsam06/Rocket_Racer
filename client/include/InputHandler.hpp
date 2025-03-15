#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "Player.hpp"
#include "Button.hpp"
#include "NetworkPackets.hpp"
#include "Physics.hpp" 

class InputHandler
{

public:
    std::vector<KeyInput> handleInput(std::vector<std::unique_ptr<Button>> &buttonList);
    bool isQuit() const { return quit; }

private:
    bool quit = false;
    KeyInput newInput;

    std::unordered_map<SDL_Keycode, Uint32> keyPressTimes;   // Stores key press timestamps
    std::unordered_map<SDL_Keycode, float> keyHoldDurations;
};

#endif
