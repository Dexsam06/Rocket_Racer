#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>

class InputHandler {
public:
    void handleInput();
    bool isQuit() const { return quit; }

private:
    bool quit = false;
};

#endif // INPUTHANDLER_H
