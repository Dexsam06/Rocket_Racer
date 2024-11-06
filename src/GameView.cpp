
#include "GameView.hpp"
#include <iostream>

GameView::GameView(int screenWidth, int screenHeight, const char *title, bool fullscreen) : screenWidth(screenWidth), screenHeight(screenHeight), fullscreen(fullscreen)
{
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) 
    {
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        isRunning = true;
    }
    else {
        isRunning = false; 
    }
}

GameView::~GameView(){}                                                          

void GameView::clear() {
    SDL_RenderClear(renderer); 
}

void GameView::present() {
    SDL_RenderPresent(renderer); 
}

void GameView::clean(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}