
#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <vector>

#include "Player.hpp"
#include "Planet.hpp"
#include "PhysicsSystem.hpp"
#include "Vector2D.hpp"
#include "Collider.hpp"
#include "RectangleCollider.hpp"
#include "CircleCollider.hpp"
#include "NetworkCommunicator.hpp"

class GameController {

    public:
        GameController(NetworkCommunicator* nc);
        ~GameController(); 
        void gameLoop();
    private: 
        std::unique_ptr<Player> player;  
        std::unique_ptr<Planet> earth; 
        std::unique_ptr<Planet> moon; 

        NetworkCommunicator* nc; 
        std::vector<std::unique_ptr<Entity>> entityList; 
        PhysicsSystem physicsSystem;

        void loadResources();
};

#endif