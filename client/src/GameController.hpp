
#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <vector>
#include <SDL2/SDL.h>

#include "NetworkCommunicator.hpp"
#include "GameView.hpp"
#include "TextureManager.hpp"
#include "Player.hpp"
#include "Planet.hpp"
#include "InputHandler.hpp"
#include "PhysicsSystem.hpp"
#include "Vector2D.hpp"
#include "Collider.hpp"
#include "RectangleCollider.hpp"
#include "CircleCollider.hpp"
#include "Button.hpp"

class GameController
{

public:
    GameController(GameView *gv, NetworkCommunicator *nc);
    ~GameController();
    void gameLoop();
    void loadResources();

private:
    void render(std::vector<Vector2D> futurePath);
    void handleIncomingEntityData(std::vector<NetworkCommunicator::EntityData> entityDataList); 
    void handleEvents(); 

    std::unique_ptr<Player> player;
    std::unique_ptr<Planet> earth;
    std::unique_ptr<Planet> moon;

    std::unique_ptr<Button> zoomButtonIn; 
    std::unique_ptr<Button> zoomButtonOut;
    std::unique_ptr<Button> resetButton;

    GameView *gv;
    NetworkCommunicator *nc;
    std::vector<std::unique_ptr<Entity>> entityList; 
    std::vector<std::unique_ptr<Button>> buttonList;
    InputHandler inputHandler;
    TextureManager textureManager;
    PhysicsSystem physicsSystem;

    void onZoomButtonClickIn();
    void onZoomButtonClickOut();
};

#endif