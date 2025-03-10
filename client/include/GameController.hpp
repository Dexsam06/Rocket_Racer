
#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <vector>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>

#include "NetworkCommunicator.hpp"
#include "GameView.hpp"
#include "InputHandler.hpp"
#include "TextureManager.hpp"
#include "Player.hpp"
#include "Physics.hpp" 
#include "RectangleCollider.hpp"
#include "PhysicsSystem.hpp"
#include "NetworkPackets.hpp"

class GameController
{

public:
    GameController(NetworkCommunicator *nc, GameView *gv);
    ~GameController();
    void gameLoop();
    void loadResources();

private:
    void handleEvents();
    void reapplyUnconfirmedInputs(); 
    void removeConfirmedInputs();
    void interpolateOtherEntities();

    std::unique_ptr<Button> zoomButtonIn; 
    std::unique_ptr<Button> zoomButtonOut;

    GameView *gv;
    NetworkCommunicator *nc;

    std::unique_ptr<Player> clientPlayer;

    std::vector<std::unique_ptr<Entity>> entityList; 
    std::vector<std::unique_ptr<Button>> buttonList; 

    InputHandler inputHandler;
    TextureManager textureManager;
    PhysicsSystem physicsSystem;

    std::vector<InputWithSequence> inputBuffer;
    int inputSequenceNumber = 0;

    void onZoomButtonClickIn();
    void onZoomButtonClickOut();

    void HandleConPlaData(ConnectedPlayersPacket & data);
    void HandleNewPlaData(NewPlayerConnectedPacket & data);
    void HandleDisPLaData(PlayerDisconnectedPacket & data);

    struct PlanetProperties {
        std::string name;
        double mass; 
        double radius;
        double startVelocityX; 
        double startVelocityY; 
    };

    double playerMass = 100.0; 

    std::unordered_map<int, PlanetProperties> planetData = {  
        {0, {"Earth", 10000000.0, 1000, 0, 0}},
        {1, {"Moon", 100000.0, 400, 150, 0}}
    };
};

#endif