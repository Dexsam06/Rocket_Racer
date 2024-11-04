
#ifndef GameView_hpp
#define GameView_hpp

#include <SDL2/SDL.h>

class GameView {

    public:
        GameView(int width, int height, const char* title, bool fullscreen);
        ~GameView();
        bool running() {return isRunning; }
        void setIsRunning(bool isRunning) {this->isRunning = isRunning; }
        void clear();
        void present();
        void clean();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        double width, height;
        bool fullscreen;
        bool isRunning; 
};

#endif