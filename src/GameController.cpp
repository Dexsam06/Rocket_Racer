
#include <GameController.hpp>
#include <InputHandler.hpp>

GameController::GameController(GameView* gv){
    loadResources();
}

GameController::~GameController(){}

void GameController::loadResources(){

}

void GameController::render(){
    gv->clear();

    gv->present();
}

void GameController::gameLoop() {
    Uint32 previousTime = SDL_GetTicks();
    while (gv->running()) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - previousTime) / 1000.0f; 
        previousTime = currentTime;

        handleEvents();
        update(deltaTime); 
        render();
        SDL_Delay(16);  
    }
    gv->clean();
}

void GameController::update(float deltaTime){ 

}

void GameController::handleEvents() {
    inputHandler.handleInput();
    if (inputHandler.isQuit()) {
        gv->setIsRunning(false);
    }
}