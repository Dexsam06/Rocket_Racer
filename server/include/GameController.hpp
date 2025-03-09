
#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>

#include "Vector2D.hpp"
#include "NetworkCommunicator.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "PhysicsSystem.hpp"
#include "NetworkPackets.hpp"

class GameController {

    public:
        GameController(NetworkCommunicator* nc);
        ~GameController(); 
        void gameLoop();
    private: 
        NetworkCommunicator* nc;    
        PhysicsSystem physicsSystem;

        std::vector<std::unique_ptr<Entity>> entityList; 

        void loadResources();

        void applyClientsInputs();

        void sendGameStatePacketToClients(); 
};

#endif