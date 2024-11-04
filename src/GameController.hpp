
#ifndef GameController_hpp
#define GameController_hpp

#include <vector>
#include <SDL2/SDL.h>
#include "GameView.hpp"

class GameController {

    public:
        GameController(GameView* gv);
        ~GameController();
        void gameLoop();
    private:
        void render();
        void loadResources();
        void update(float deltaTime); 
        void handleEvents();

        Player* player;  
        PLanet* earth; 
        GameView* gv;
        std::vector<Entity*> entityList;
        InputHandler inputHandler;
};

#endif