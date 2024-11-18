
#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <SDL2/SDL.h>
#include "Entity.hpp"
#include "Vector2D.hpp"
#include <SDL2/SDL_ttf.h>
#include "Button.hpp"

class GameView {

    public:
        GameView(int screenWidth, int screenHeight, const char* title, bool fullscreen);
        ~GameView();
        void render(std::vector<Entity *> entityList, std::vector<Button *> buttonList);
        void clean();
        void drawBackground(Entity* player);

        bool running() {return isRunning; } 
        void setIsRunning(bool isRunning) {this->isRunning = isRunning; }
        SDL_Renderer* getRenderer() {return renderer; }
        TTF_Font* getFont() {return font; }
        double getScreenWidth() {return screenWidth; }
        double getScreenHeight() {return screenHeight; }
        void setBackground(SDL_Texture* background) {this->background = background; }
        void setScalingFactors(Vector2D scalingFactor) {this->scalingFactor += scalingFactor; } 
    private:
        SDL_Texture* background; 
        SDL_Window* window;
        SDL_Renderer* renderer;
        TTF_Font* font;
        double screenWidth, screenHeight;
        bool fullscreen;
        bool isRunning; 
        Vector2D scalingFactor;
};

#endif