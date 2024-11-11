
#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <SDL2/SDL.h>
#include "Player.hpp"
#include "Vector2D.hpp"

class GameView {

    public:
        GameView(int screenWidth, int screenHeight, const char* title, bool fullscreen);
        ~GameView();
        void clear();
        void present();
        void setScalingFactor(double xScale, double yScale);
        void render(Entity *entity, Vector2D playerPos);
        void clean();
        void drawBackground(Player* player);

        bool running() {return isRunning; } 
        void setIsRunning(bool isRunning) {this->isRunning = isRunning; }
        SDL_Renderer* getRenderer() {return renderer; }
        double getScreenWidth() {return screenWidth; }
        double getScreenHeight() {return screenHeight; }
        void setBackground(SDL_Texture* background) {this->background = background; }
    private:
        SDL_Texture* background; 
        SDL_Window* window;
        SDL_Renderer* renderer;
        double screenWidth, screenHeight;
        bool fullscreen;
        bool isRunning; 
};

#endif