
#include "../include/NetworkCommunicator.hpp"
#include "../include/GameView.hpp"
#include "../include/GameController.hpp"

int main(int argc, const char * argv[])
{
    std::cout << "Starting Client..." << std::endl;

    const int windowWidth = 1920;
    const int windowHeight = 1080;
    const char* windowTitle = "Rocket Racer";

    GameView gv(windowWidth, windowHeight, windowTitle, false); 
    
    NetworkCommunicator nc;

    GameController gc(&nc, &gv); 

    gc.gameLoop();

    return 0; 

    
}