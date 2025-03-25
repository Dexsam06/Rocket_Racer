
#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <vector>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <thread>

#include "GameView.hpp"
#include "InputHandler.hpp"
#include "TextureManager.hpp"
#include "Player.hpp"
#include "Planet.hpp"
#include "Physics.hpp" 
#include "CircleCollider.hpp"
#include "RectangleCollider.hpp"
#include "PhysicsSystem.hpp"
#include "NetworkPackets.hpp"

class NetworkCommunicator;

class GameController
{

public:
    GameController(NetworkCommunicator *nc, GameView *gv, std::string &resourcePath);
    ~GameController();
    void gameLoop();
    void loadResources();

private:
    void handleEvents();
    void interpolateOtherEntities();

    std::unique_ptr<Button> zoomButtonIn; 
    std::unique_ptr<Button> zoomButtonOut;

    GameView *gv;
    NetworkCommunicator *nc; 

    std::string resourcePath;

    Player* clientPlayer;

    std::vector<std::unique_ptr<Entity>> entityList; 
    std::vector<std::unique_ptr<Button>> buttonList; 

    InputHandler inputHandler;
    TextureManager textureManager;
    PhysicsSystem physicsSystem;

    void onZoomButtonClickIn();
    void onZoomButtonClickOut();

    void HandleConPlaData(ConnectedPlayersPacket &data);
    void HandleNewPlaData(NewPlayerConnectedPacket &data);
    void HandleDisPLaData(PlayerDisconnectedPacket &data);

    void HandleInputEvent(ClientInputPacket &data);
};

#endif