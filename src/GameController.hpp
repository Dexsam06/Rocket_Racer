
#ifndef GameController_hpp
#define GameController_hpp

#include <vector>
#include <SDL2/SDL.h>
#include <GameView.hpp>
#include <TextureManager.hpp>
#include <Player.hpp>
#include <Planet.hpp>

class GameController {

    public:
        GameController(GameView* gv);
        ~GameController();
        void gameLoop();
    private:
        void render();
        void loadResources();
        void update(double deltaTime); 
        void handleEvents();

        Player* player;  
        Planet* earth; 
        GameView* gv;
        std::vector<Entity*> entityList;
        InputHandler inputHandler;
        TextureManager textureManager;
};

#endif