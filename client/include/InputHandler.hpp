#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "Player.hpp"
#include "Button.hpp"

class InputHandler
{

public:
    struct keyInput
    {
        int keyCode;
        float duration;
    };

    std::vector<keyInput> handleInput(std::vector<std::unique_ptr<Button>> &buttonList);
    bool isQuit() const { return quit; }

private:
    bool quit = false;
    keyInput newInput;
};

#endif
