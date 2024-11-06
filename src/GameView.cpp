
#include "GameView.hpp"
#include <iostream>

GameView::GameView(int screenWidth, int screenHeight, const char *title, bool fullscreen)
    : screenWidth(screenWidth), screenHeight(screenHeight), fullscreen(fullscreen), isRunning(false)
{
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    isRunning = true;  
}


GameView::~GameView() {}                                                       

void GameView::clear() {
    if (renderer) {
        SDL_RenderClear(renderer); 
    }
}

void GameView::present() {
    if (renderer) {
        SDL_RenderPresent(renderer); 
    }
}

void GameView::clean() {
    if (renderer) {
        SDL_DestroyRenderer(renderer); 
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}

SDL_Renderer* GameView::getRenderer() { 
    return renderer;
}
