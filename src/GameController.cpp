
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
    Vector2D playerPos(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2);
    Vector2D playerVel(0, 0);
    player = new Player(playerTexture, playerPos, playerVel, 100); 
    player->setPlayerWidth(width);
    player->setPlayerHeight(height); 
    entityList.push_back(player);

    //Planets
    Vector2D earthPos(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 + 300); 
    Vector2D earthVel(0, 0);
    earth = new Planet(earthPos, earthVel, 2000000, 300); 
    entityList.push_back(earth);

    Vector2D moonPos(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 - 400); 
    Vector2D moonVel(100, 0);
    moon = new Planet(moonPos, moonVel, 20000, 100); 
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
    int updateTime = (int)((1/fps) * 1000.0); 
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