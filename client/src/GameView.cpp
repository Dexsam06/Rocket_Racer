
#include "../include/GameView.hpp"
#include <iostream>

GameView::GameView(int screenWidth, int screenHeight, const char *title, std::string resourcePath, bool fullscreen)
    : screenWidth(screenWidth), screenHeight(screenHeight), fullscreen(fullscreen), isRunning(false), scalingFactor(1.0, 1.0), resourcePath(resourcePath)
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

    setupFireExhaustSpriteSheet();

    isRunning = true;
}

GameView::~GameView() {}

void GameView::render(std::vector<std::unique_ptr<Entity>> &entityList, std::vector<std::unique_ptr<Button>> &buttonList)
{
    frame = (SDL_GetTicks() / 100) % FRAME_COUNT; // Change frames every 100ms

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Player *clientPlayer = dynamic_cast<Player *>(entityList[0].get());
    Vector2D clientPlayerPos = clientPlayer->getPosition();

    drawBackground(clientPlayer);
    drawLeaderBoard(entityList);
    drawClientPlayer(clientPlayer);

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
            drawPlayerUsername(player->getPosition(), clientPlayerPos, player->getUsername());
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

void GameView::drawLeaderBoard(std::vector<std::unique_ptr<Entity>> &entityList)
{
    const char *text = "Leaderboard";
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, {255, 255, 255, 255});
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
    SDL_Rect textRect = {0.85 * screenWidth, 0.1 * screenHeight, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);

    Vector2D moonPos;

    for (int i = 0; i < entityList.size(); i++)
    {
        if (entityList[i]->getID() == 1001)
        {
            moonPos = entityList[i]->getPosition();
            break;
        }
    }

    std::vector<std::pair<int, std::string>> leaderBoardData;

    for (int i = 0; i < entityList.size(); i++)
    {
        Player *player = dynamic_cast<Player *>(entityList[i].get()); 
        if (player)
        {
            Vector2D delta = player->getPosition() - moonPos;
            int distance = static_cast<int>(delta.magnitude()) - 900;
            std::string text = player->getUsername();
            leaderBoardData.push_back({distance, text});
        }
    }

    std::sort(leaderBoardData.begin(), leaderBoardData.end(),
              [](const std::pair<int, std::string> &a, const std::pair<int, std::string> &b)
              {
                  return a.first < b.first;
              });

    for (int i = 0; i < leaderBoardData.size(); i++)
    {
        std::string text = std::to_string((i + 1)) + ": " + leaderBoardData[i].second + " - " + std::to_string(leaderBoardData[i].first) + " meters";
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), {255, 255, 255, 255});
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
        SDL_Rect textRect = {0.85 * screenWidth, (0.1 * screenHeight) + ((i + 1) * 50), textWidth, textHeight};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect); 
        SDL_DestroyTexture(textTexture);
    }
}

void GameView::drawClientPlayer(Player *clientPlayer)
{
    // Draw client player
    int scaledWidth = static_cast<int>(clientPlayer->getPlayerWidth() * scalingFactor.x);
    int scaledHeight = static_cast<int>(clientPlayer->getPlayerHeight() * scalingFactor.y);

    SDL_Rect playerDestRect = {
        screenWidth / 2 - scaledWidth / 2,
        screenHeight / 2 - scaledHeight / 2,
        scaledWidth,
        scaledHeight};
    SDL_RenderCopyEx(renderer, clientPlayer->getTexture(), nullptr, &playerDestRect, clientPlayer->getRotation(), nullptr, SDL_FLIP_NONE);
    drawPlayerUsername(clientPlayer->getPosition(), clientPlayer->getPosition(), clientPlayer->getUsername());
    if (clientPlayer->getisThrusting() == true)
    {
        drawExhaustAnimation(clientPlayer->getPosition(), clientPlayer->getPosition(), clientPlayer->getRotation());
    }
}

void GameView::drawExhaustAnimation(Vector2D currentPlayerPosition, Vector2D playerClientPos, double rotation)
{
    Vector2D screenCenter(screenWidth / 2, screenHeight / 2);

    Vector2D offsetFromClientPlayer(
        currentPlayerPosition.x - playerClientPos.x,
        currentPlayerPosition.y - playerClientPos.y);

    Vector2D scaledOffset(
        offsetFromClientPlayer.x * scalingFactor.x,
        offsetFromClientPlayer.y * scalingFactor.y);

    Vector2D scaledPosition(
        screenCenter.x + scaledOffset.x,
        screenCenter.y + scaledOffset.y);

    const double ROCKET_HEIGHT = 250 * scalingFactor.y;

    double theta = rotation * M_PI / 180.0;

    double fireX = scaledPosition.x - (sin(theta) * ROCKET_HEIGHT);
    double fireY = scaledPosition.y + (cos(theta) * ROCKET_HEIGHT);

    // Scale width/height for flame
    int scaledWidth = static_cast<int>(FRAME_WIDTH * scalingFactor.x) * 0.35;
    int scaledHeight = static_cast<int>(FRAME_HEIGHT * scalingFactor.y) * 0.6;

    // Define SDL_Rect (centered at computed position)
    SDL_Rect destRect = {
        static_cast<int>(fireX - scaledWidth / 2),  // Center horizontally
        static_cast<int>(fireY - scaledHeight / 2), // Center vertically 3
        scaledWidth,
        scaledHeight};

    // Correctly rotate flame (opposite to rocket)
    SDL_RenderCopyEx(renderer, fireTexture, &spriteClips[frame], &destRect, rotation + 180, nullptr, SDL_FLIP_NONE);
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

void GameView::drawPlayerUsername(Vector2D currentPlayerPosition, Vector2D clientPlayerPosition, const std::string &username)
{
    Vector2D screenCenter(screenWidth / 2, screenHeight / 2);

    Vector2D offsetFromClientPlayer(
        currentPlayerPosition.x - clientPlayerPosition.x,
        currentPlayerPosition.y - clientPlayerPosition.y);

    Vector2D scaledOffset(
        offsetFromClientPlayer.x * scalingFactor.x,
        offsetFromClientPlayer.y * scalingFactor.y);

    int textPositionX = screenCenter.x + scaledOffset.x;
    int textPositionY = screenCenter.y + scaledOffset.y;

    SDL_Color textColor = {0, 255, 0, 255};
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

    SDL_Rect textRect = {textPositionX - textSurface->w / 2, textPositionY - textSurface->h / 2, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void GameView::setupFireExhaustSpriteSheet()
{
    std::string path = resourcePath + "fireSpriteSheet.png";
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface)
    {
        std::cerr << "Failed to load fire sprite sheet! SDL_image Error: " << IMG_GetError() << std::endl;
        return;
    }

    fireTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    if (!fireTexture)
    {
        std::cerr << "Failed to create fire texture! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    for (int i = 0; i < FRAME_COUNT; i++)
    {
        spriteClips[i].x = i * FRAME_WIDTH;
        spriteClips[i].y = 0;
        spriteClips[i].w = FRAME_WIDTH;
        spriteClips[i].h = FRAME_HEIGHT;
    }
}