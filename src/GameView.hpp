
#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <SDL2/SDL.h>

class GameView {

    public:
        GameView(int screenWidth, int screenHeight, const char* title, bool fullscreen);
        ~GameView();
        bool running() {return isRunning; }
        void setIsRunning(bool isRunning) {this->isRunning = isRunning; }
        SDL_Renderer* getRenderer();
        double getScreenWidth() {return screenWidth; }
        double getScreenHeight() {return screenHeight; }
        void clear();
        void present();
        void setScalingFactor(double xScale, double yScale);
        void clean();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        double screenWidth, screenHeight;
        bool fullscreen;
        bool isRunning; 
};

#endif