
#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

#include "Entity.hpp"
#include "Player.hpp"
#include "Vector2D.hpp"
#include "Button.hpp"

class GameView {

    public:
        GameView(int screenWidth, int screenHeight, const char* title, std::string resourcePath, bool fullscreen);
        ~GameView();
        void render(std::vector<std::unique_ptr<Entity>>& entityList, std::vector<std::unique_ptr<Button>>& buttonList);
        void clean();
        
        bool running() {return isRunning; } 
        void setIsRunning(bool isRunning) {this->isRunning = isRunning; }
        SDL_Renderer* getRenderer() {return renderer; }
        TTF_Font* getFont() {return font; }
        double getScreenWidth() {return screenWidth; }
        double getScreenHeight() {return screenHeight; }
        void setBackground(SDL_Texture* background) {this->background = background; }
        void setScalingFactors(Vector2D scalingFactor) {this->scalingFactor *= scalingFactor; } 
    private:
        SDL_Texture* background;  
        SDL_Window* window;
        SDL_Renderer* renderer;
        TTF_Font* font;
        double screenWidth, screenHeight;
        bool fullscreen;
        bool isRunning; 
        Vector2D scalingFactor;
        std::string resourcePath;

        void drawPlayerUsername(int &x, int &y, const std::string &username);
        void drawBackground(Player* clientPlayer);
        void drawFuturePath(std::vector<Vector2D>& futurePath, Vector2D playerPos);
        std::vector<SDL_Point> convertToSDLPoints(const std::vector<Vector2D>& points);

        void drawExhaustAnimation(Vector2D currentPlayerPosition, Vector2D playerClientPos, double rotation);
        void setupFireExhaustSpriteSheet();

        static const int FRAME_COUNT = 4;
        const int FRAME_WIDTH = 214; 
        const int FRAME_HEIGHT = 291;
        int frame = 0;

        SDL_Texture* fireTexture = nullptr;
        SDL_Rect spriteClips[FRAME_COUNT]; 
};

#endif