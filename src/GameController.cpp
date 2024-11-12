
#include "GameController.hpp"
#include "Physics.hpp"
#include <iostream>

GameController::GameController(GameView *gv) : gv(gv)
{
    loadResources();
}

GameController::~GameController() {}

void GameController::loadResources() 
{
    //Background
    SDL_Texture* backgroundTexture = textureManager.loadTexture("background", "../res/spaceBackgroundTiling.jpg", gv->getRenderer()); 
    gv->setBackground(backgroundTexture);

    //Player
    int height, width; 
    SDL_Texture* playerTexture = textureManager.loadTexture("player", "../res/apollo11.png", gv->getRenderer()); 
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);
    
    player = new Player(std::make_unique<RectangleCollider>(Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2), width, height, 0), playerTexture, Vector2D (gv->getScreenWidth() / 2, gv->getScreenHeight() / 2), Vector2D (0, 0), 100.0); 
    player->setPlayerWidth(width); 
    player->setPlayerHeight(height); 
    entityList.push_back(player);     

    //Planets
    earth = new Planet(std::make_unique<CircleCollider>(Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 + 300), 300.0), Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 + 300), Vector2D (0, 0), 2000000.0, 300.0); 
    entityList.push_back(earth);

    moon = new Planet(std::make_unique<CircleCollider>(Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 - 500), 100.0), Vector2D (gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 - 500), Vector2D (100, 0), 20000.0, 100.0); 
    entityList.push_back(moon);
}  

void GameController::render() 
{
    SDL_SetRenderDrawColor(gv->getRenderer(), 0, 0, 0, 255); 
    gv->clear();
    gv->drawBackground(player); 
    for (Entity *entity : entityList) 
    {
        gv->render(entity, player->getPosition()); 
    }
    gv->present();
}

void GameController::gameLoop()
{
    int fps = 60;
    int updateTime = (int)((1/fps) * 1000); 
    Uint32 previousTime = SDL_GetTicks();
    while (gv->running())
    {
        long deltaTime = (SDL_GetTicks() - previousTime); 
        
        if(deltaTime > updateTime) {
            previousTime = SDL_GetTicks();
            handleEvents();
            physicsSystem.update(entityList, deltaTime / 1000.0);
            render();
        }
    }
    gv->clean();
}

void GameController::handleEvents()
{
    inputHandler.handleInput(player); 
    if (inputHandler.isQuit())
    {
        gv->setIsRunning(false);
    }
}