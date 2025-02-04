

#include "GameController.hpp"
#include "Physics.hpp"
#include <iostream>
#include <chrono>

GameController::GameController(NetworkCommunicator *nc) : nc(nc) 
{
    loadResources(); 
}

GameController::~GameController() {}

void GameController::gameLoop()
{
    nc->setEntityList(&entityList); 
    int fps = 12;
    int updateTime = (int)((1.0 / fps) * 1000); // Milliseconds per frame
    auto previousTime = std::chrono::steady_clock::now();

    while (nc->serverRunning())
    {
        auto currentTime = std::chrono::steady_clock::now();
        long deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();

        if (deltaTime > updateTime)
        {
            previousTime = std::chrono::steady_clock::now();
            nc->NetworkHandler();
            physicsSystem.update(entityList, deltaTime / 1000.0); // Convert to seconds
        }
    }
}

void GameController::loadResources() 
{
    // Planets
    earth = std::make_unique<Planet>(
        std::make_unique<CircleCollider>( 
            Vector2D(700, 1500),
            1000.0),
        Vector2D(700, 1500),    
        Vector2D(0, 0),
        10000000.0,
        1000.0
        );
    entityList.push_back(std::move(earth)); 

    moon = std::make_unique<Planet>(
        std::make_unique<CircleCollider>(
            Vector2D(900, 100), 
            400.0), 
        Vector2D(900, 100), 
        Vector2D(150, 0), 
        100000.0, 
        400.0
        );
    entityList.push_back(std::move(moon));
}


