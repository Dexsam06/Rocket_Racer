#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>
#include "Player.hpp"

class InputHandler {
public:
    void handleInput(Player* player);
    bool isQuit() const { return quit; }

private:
    bool quit = false;
};

#endif // INPUTHANDLER_H
