
#include <GameController.hpp>
#include <GameView.hpp>

int main(int argc, const char * argv[]) {

    const int windowWidth = 1400;
    const int windowHeight = 1000;
    const char* windowTitle = "Rocket Racer";

    GameView gv(windowWidth, windowHeight, windowTitle, false);
    GameController gc(&gv);

    gc.gameLoop();

    return 0;
}