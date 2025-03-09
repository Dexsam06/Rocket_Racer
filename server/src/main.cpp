#include "../include/NetworkCommunicator.hpp"
#include "../include/GameController.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
    std::cout << "Starting server..." << std::endl;

    NetworkCommunicator nc;

    GameController gc(&nc);

    gc.gameLoop();

    return 0; 
}