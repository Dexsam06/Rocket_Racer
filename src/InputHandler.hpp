#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>
#include "Player.hpp"
#include "Button.hpp"

class InputHandler {
public:
    void handleInput(Player* player, std::vector<Button*> buttonList);
    bool isQuit() const { return quit; }

private:
    bool quit = false;
};

#endif // INPUTHANDLER_H
