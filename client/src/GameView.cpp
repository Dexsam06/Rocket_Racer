
#include "../include/GameView.hpp"
#include <iostream>

GameView::GameView(int screenWidth, int screenHeight, const char *title, std::string resourcePath, bool fullscreen)
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
    if (!renderer || renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    std::string fontPath = resourcePath + "Fonts/Roboto-Regular.ttf";
    font = TTF_OpenFont(fontPath.c_str(), 24); 
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

void GameView::render(std::vector<std::unique_ptr<Entity>> &entityList, std::vector<std::unique_ptr<Button>> &buttonList)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Player *clientPlayer = dynamic_cast<Player *>(entityList[0].get());
    drawBackground(clientPlayer);
    Vector2D clientPlayerPos = clientPlayer->getPosition();

    // Draw client player
    int scaledWidth = static_cast<int>(clientPlayer->getPlayerWidth() * scalingFactor.x);
    int scaledHeight = static_cast<int>(clientPlayer->getPlayerHeight() * scalingFactor.y);

    SDL_Rect playerDestRect = {
        screenWidth / 2 - scaledWidth / 2,
        screenHeight / 2 - scaledHeight / 2,
        scaledWidth,
        scaledHeight};
    SDL_RenderCopyEx(renderer, clientPlayer->getTexture(), nullptr, &playerDestRect, clientPlayer->getRotation(), nullptr, SDL_FLIP_NONE);
    int textPositionX = screenWidth / 2 - (45 / 2) * scalingFactor.x;
    int textPositionY = (screenHeight / 2) + 200 * scalingFactor.y;
    drawPlayerUsername(textPositionX, textPositionY, clientPlayer->getUsername());

    // Draw other entities
    for (int i = 1; i < entityList.size(); i++)
    {
        if (!entityList[i])
        {
            std::cerr << "Warning: Null entity found in entityList!" << std::endl;
            continue;
        }
        SDL_Texture *texture = entityList[i]->getTexture();
        if (!texture || texture == nullptr)
        {
            std::cerr << "Warning: Entity has null texture!" << std::endl;
            continue;
        }
        entityList[i]->draw(renderer, screenWidth, screenHeight, clientPlayerPos, scalingFactor);

        if (Player *player = dynamic_cast<Player *>(entityList[i].get()))
        {
            Vector2D screenCenter(screenWidth / 2, screenHeight / 2);

            Vector2D offsetFromClientPlayer(
                player->getPosition().x - clientPlayerPos.x,
                player->getPosition().y - clientPlayerPos.y);

            Vector2D scaledOffset(
                offsetFromClientPlayer.x * scalingFactor.x,
                offsetFromClientPlayer.y * scalingFactor.y);

            textPositionX = screenCenter.x + scaledOffset.x,
            textPositionY = screenCenter.y + scaledOffset.y;
            drawPlayerUsername(textPositionX, textPositionY, player->getUsername()); 
        } 
    }

    for (std::unique_ptr<Button> &button : buttonList)
    {
        if (!button)
        {
            std::cerr << "Warning: Null button found in buttonList!" << std::endl;
            continue;
        }
        button->render();
    }

    // drawFuturePath(futurePath, clientPlayerPos);
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

void GameView::drawBackground(Player *clientPlayer)
{
    int bgWidth, bgHeight;
    SDL_QueryTexture(background, nullptr, nullptr, &bgWidth, &bgHeight);

    double xScale = 1 + (std::pow(std::abs(clientPlayer->getVelocity().x), 2) / 1000000.0);
    double yScale = 1 + (std::pow(std::abs(clientPlayer->getVelocity().y), 2) / 1000000.0);

    SDL_RenderSetScale(renderer, xScale, yScale);

    int offsetX = static_cast<int>(clientPlayer->getPosition().x) % bgWidth;
    int offsetY = static_cast<int>(clientPlayer->getPosition().y) % bgHeight;

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

void GameView::drawFuturePath(std::vector<Vector2D> &futurePath, Vector2D playerPos)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    std::vector<Vector2D> adjustedPathPlayer;
    std::vector<Vector2D> adjustedPathMoon;
    int count = 0;
    for (const auto &point : futurePath)
    {
        Vector2D adjusted = point - playerPos;
        adjusted.x *= scalingFactor.x;
        adjusted.y *= scalingFactor.y;
        adjusted += Vector2D(screenWidth / 2, screenHeight / 2);
        if (count % 2 == 0)
        {
            adjustedPathPlayer.push_back(adjusted);
        }
        else
        {
            adjustedPathMoon.push_back(adjusted);
        }
        count++;
    }

    // Player future path
    auto sdlPoints = convertToSDLPoints(adjustedPathPlayer);
    SDL_RenderDrawLines(renderer, sdlPoints.data(), futurePath.size() / 2);

    // Moon future path
    sdlPoints = convertToSDLPoints(adjustedPathMoon);
    SDL_RenderDrawLines(renderer, sdlPoints.data(), futurePath.size() / 2);
}

void GameView::drawPlayerUsername(int &x, int &y, const std::string &username)
{
    SDL_Color textColor = {255, 255, 255, 255}; // White color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, username.c_str(), textColor);
    if (!textSurface)
    {
        std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture)
    {
        std::cerr << "Unable to create texture from text surface! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

std::vector<SDL_Point> GameView::convertToSDLPoints(const std::vector<Vector2D> &points)
{
    std::vector<SDL_Point> sdlPoints;
    sdlPoints.reserve(points.size());

    for (const auto &point : points)
    {
        sdlPoints.push_back(SDL_Point{static_cast<int>(point.x), static_cast<int>(point.y)});
    }

    return sdlPoints;
}
