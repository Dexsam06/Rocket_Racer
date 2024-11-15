
#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <vector>
#include <SDL2/SDL.h>

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

class GameController {

    public:
        GameController(GameView* gv);
        ~GameController(); 
        void gameLoop();
    private:
        void render();
        void loadResources();
        void handleEvents();

        Player* player;  
        Planet* earth; 
        Planet* moon;

        Button* zoomButtonIn;
        Button* zoomButtonOut;

        GameView* gv;   
        std::vector<Entity*> entityList; 
        std::vector<Button*> buttonList;
        InputHandler inputHandler;
        TextureManager textureManager;
        PhysicsSystem physicsSystem;

        void onZoomButtonClickIn();
        void onZoomButtonClickOut();
};

#endif