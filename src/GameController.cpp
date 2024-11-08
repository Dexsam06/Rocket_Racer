
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
    SDL_Texture* backgroundTexture = textureManager.loadTexture("background", "../res/spaceBackgroundTiling.jpg", gv->getRenderer()); 
    gv->setBackground(backgroundTexture);

    int height, width;
    SDL_Texture* playerTexture = textureManager.loadTexture("player", "../res/apollo11.png", gv->getRenderer()); 
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

    player = new Player(playerTexture, gv->getScreenWidth() / 2, gv->getScreenHeight() / 2, 100, 0, 0); 
    player->setPlayerWidth(width);
    player->setPlayerHeight(height); 
    entityList.push_back(player);

    earth = new Planet(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 + 300, 300, 2000000, 0, 0); 
    entityList.push_back(earth);

    moon = new Planet(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 - 400, 100, 20000, 100, 0); 
    entityList.push_back(moon);
} 

void GameController::render()
{
    SDL_SetRenderDrawColor(gv->getRenderer(), 0, 0, 0, 255); 
    gv->clear();
    gv->drawBackground(player); 
    for (Entity *entity : entityList)
    {
        if (typeid(*entity) == typeid(Planet))
        {
            entity->setAnotherEntityXPos(entityList[0]->getXPos()); 
            entity->setAnotherEntityYPos(entityList[0]->getYPos());
        }
        entity->draw(gv->getRenderer(), gv->getScreenWidth(), gv->getScreenHeight());
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
            update(deltaTime / 1000.0);
            render();
        }
    }
    gv->clean();
}

void GameController::update(double deltaTime)
{
    std::vector<std::vector<double>> entityData;
    for (const auto &entity : entityList)
    {
        entityData.push_back({entity->getMass(), entity->getXPos(), entity->getYPos()}); 
    }

    for (size_t i = 0; i < entityList.size(); ++i)
    {
        std::vector<std::vector<double>> entityDataFinal; 

        for (size_t j = 0; j < entityList.size(); ++j) 
        {
            if (i == j)
                continue; 
            entityDataFinal.push_back(entityData[j]);
        }

        entityList[i]->calculatePhysics(entityDataFinal, deltaTime);
    }
}

void GameController::handleEvents()
{
    inputHandler.handleInput(player);
    if (inputHandler.isQuit())
    {
        gv->setIsRunning(false);
    }
}