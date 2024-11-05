
#include <GameController.hpp>
#include <InputHandler.hpp>
#include <iostream>

GameController::GameController(GameView* gv){
    loadResources();
}

GameController::~GameController(){}

void GameController::loadResources(){
    int height, width;
    SDL_Texture* playerTexture = textureManager.loadTexture("player", "../res/apollo11.png", gv->getRenderer()); 
    if (SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height) != 0) {
            std::cerr << "SDL_QueryTexture Error: " << SDL_GetError() << std::endl;
            width = height = 0; 
        }
    player = new Player(playerTexture, gv->getScreenWidth() / 2 - width / 2, gv->getScreenHeight() / 2, 300, 0, 0);
    player->setPlayerWidth(width);
    player->setPlayerHeight(height);
    entityList.push_back(player);

    earth = new Planet(2000, gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2, 10000, 0, 0);
    entityList.push_back(earth);
}

void GameController::render(){
    SDL_SetRenderDrawColor(gv->getRenderer(), 0, 0, 0, 255); 
    gv->clear();
    for(Entity* entity : entityList) {
        entity->draw(gv->getRenderer(), gv->getScreenWidth(), gv->getScreenHeight());
    }
    gv->present();
}

void GameController::gameLoop() {
    Uint32 previousTime = SDL_GetTicks();
    while (gv->running()) {
        Uint32 currentTime = SDL_GetTicks();
        double deltaTime = (currentTime - previousTime) / 1000.0; 
        previousTime = currentTime;

        handleEvents();
        update(deltaTime); 
        render();
        SDL_Delay(16);  
    }
    gv->clean();
}

void GameController::update(double deltaTime) { 
    std::vector<std::vector<double>> data;  

    for (Entity* entity : entityList) {
        if (typeid(*entity) == typeid(Planet)) {
            std::vector<double> planetData;
            planetData.push_back(entity->getMass());   
            planetData.push_back(entity->getXPos());   
            planetData.push_back(entity->getYPos());  
            data.push_back(planetData);
        }
    }
    player->calculatePhysics(data, deltaTime); 
}


void GameController::handleEvents() { 
    inputHandler.handleInput(player);
    if (inputHandler.isQuit()) {
        gv->setIsRunning(false);
    }
}