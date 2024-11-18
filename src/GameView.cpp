
#include "GameView.hpp"
#include "Player.hpp"
#include <iostream>

GameView::GameView(int screenWidth, int screenHeight, const char *title, bool fullscreen)
    : screenWidth(screenWidth), screenHeight(screenHeight), fullscreen(fullscreen), isRunning(false), scalingFactor(1.0, 1.0)
{
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    font = TTF_OpenFont("../res/Fonts/Roboto-Regular.ttf", 24);
    if (!font)
    {
        std::cerr << "Font Load Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return;
    }

    isRunning = true;
}

GameView::~GameView() {}

void GameView::render(std::vector<Entity *> entityList, std::vector<Button *> buttonList)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    drawBackground(entityList[0]);

    SDL_RenderSetScale(renderer, scalingFactor.x, scalingFactor.y);

    for (Entity *entity : entityList)
    {
        entity->draw(renderer, screenWidth, screenHeight, entityList[0]->getPosition());
    }

    SDL_RenderSetScale(renderer, 1.0, 1.0); 

    for (Button *button : buttonList)
    {
        button->render();
    }

    SDL_RenderPresent(renderer);
}

void GameView::clean()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}

void GameView::drawBackground(Entity *player)
{
    int bgWidth, bgHeight;
    SDL_QueryTexture(background, nullptr, nullptr, &bgWidth, &bgHeight);

    double xScale = 1 + (std::pow(std::abs(player->getVelocity().x), 2) / 1000000.0);
    double yScale = 1 + (std::pow(std::abs(player->getVelocity().y), 2) / 1000000.0);

    SDL_RenderSetScale(renderer, xScale, yScale);

    int offsetX = static_cast<int>(player->getPosition().x) % bgWidth;
    int offsetY = static_cast<int>(player->getPosition().y) % bgHeight;

    int startX = -((offsetX % bgWidth) + bgWidth) % bgWidth;
    int startY = -((offsetY % bgHeight) + bgHeight) % bgHeight;

    for (int x = startX; x < screenWidth; x += bgWidth)
    {
        for (int y = startY; y < screenHeight; y += bgHeight)
        {
            SDL_Rect destRect = {x, y, bgWidth, bgHeight};
            SDL_RenderCopy(renderer, background, nullptr, &destRect);
        }
    }

    SDL_RenderSetScale(renderer, 1.0, 1.0);
}