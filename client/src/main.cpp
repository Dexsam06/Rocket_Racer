
#include "../include/NetworkCommunicator.hpp"
#include "../include/GameView.hpp"
#include "../include/GameController.hpp"
#include "../include/UserInputWindow.hpp"

int main(int argc, const char * argv[])
{
    std::cout << "Starting Client..." << std::endl;

    std::string serverIP, username;

    if (!getUserInput(serverIP, username)) {
        std::cerr << "User canceled input. Exiting..." << std::endl;
        return 1; 
    } 

    NetworkCommunicator nc(serverIP, username);

    const int windowWidth = 1920;
    const int windowHeight = 1080;
    const char* windowTitle = "Rocket Racer"; 

    GameView gv(windowWidth, windowHeight, windowTitle, false); 
    
    GameController gc(&nc, &gv); 

    gc.gameLoop();

    return 0; 

    
}