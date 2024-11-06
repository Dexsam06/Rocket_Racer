
#include "GameController.hpp"
#include "GameView.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
    std::cout << "Starting game..." << std::endl;

    const int windowWidth = 1400;
    const int windowHeight = 1000;
    const char* windowTitle = "Rocket Racer";

    GameView gv(windowWidth, windowHeight, windowTitle, false);

    if (!gv.running()) {
        std::cerr << "Error: GameView initialization failed!" << std::endl;
        return -1;  // Return an error code to indicate failure
    }

    GameController gc(&gv);

    gc.gameLoop();

    return 0; 
}