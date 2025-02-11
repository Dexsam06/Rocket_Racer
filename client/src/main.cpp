
#include "NetworkCommunicator.hpp"
#include "GameController.hpp"
#include "GameView.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
    std::cout << "Starting Client..." << std::endl;

    const int windowWidth = 1920;
    const int windowHeight = 1080;
    const char* windowTitle = "Rocket Racer";

    GameView gv(windowWidth, windowHeight, windowTitle, false); 

    std::string ipAddress;
    std::cout << "Skriv ip-address: " << std::endl; 
    std::cin >> ipAddress; 

    NetworkCommunicator nc(ipAddress); 

    GameController gc(&gv, &nc);

    gc.gameLoop();

    return 0; 
}