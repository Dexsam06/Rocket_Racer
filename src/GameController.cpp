
#include "GameController.hpp"
#include "Physics.hpp"
#include <iostream>

GameController::GameController(GameView *gv) : gv(gv)
{
    loadResources();
}

GameController::~GameController() {}

void GameController::onZoomButtonClickOut()
{
    gv->setScalingFactors(Vector2D(0.9, 0.9));
}
void GameController::onZoomButtonClickIn()
{
    gv->setScalingFactors(Vector2D(1.1, 1.1));
}
void GameController::ResetPlayerButton()
{
    player->setPosition(Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 - 30));
    player->setVelocity(Vector2D(0, 0));
    player->setRotationSpeed(-player->getRotationSpeed());
    player->setRotation(0);
}

void GameController::loadResources()
{
    // Background
    SDL_Texture *backgroundTexture = textureManager.loadTexture("background", "../res/spaceBackgroundTiling.jpg", gv->getRenderer());
    gv->setBackground(backgroundTexture);

    // Player
    int height, width;
    SDL_Texture *playerTexture = textureManager.loadTexture("player", "../res/apollo11.png", gv->getRenderer());
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

    player = std::make_unique<Player>(
        std::make_unique<RectangleCollider>(
            Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2),
            width,
            height,
            0),
        playerTexture,
        Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2),
        Vector2D(0, 0),
        100.0);
    player->setPlayerWidth(width);
    player->setPlayerHeight(height);
    entityList.push_back(std::move(player));

    // Planets
    earth = std::make_unique<Planet>(
        std::make_unique<CircleCollider>(
            Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 + 300),
            300.0),
        Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 + 300),
        Vector2D(0, 0),
        2000000.0,
        300.0,
        0,
        250,
        0);
    entityList.push_back(std::move(earth));

    moon = std::make_unique<Planet>(
        std::make_unique<CircleCollider>(
            Vector2D(gv->getScreenWidth() / 2, gv->getScreenHeight() / 2 + height / 2 - 500),
            150.0),
        Vector2D(gv->getScreenWidth() / 2 + 300, gv->getScreenHeight() / 2 + height / 2 - 500),
        Vector2D(100, 0),
        20000.0,
        150.0,
        194,
        197,
        204);
    entityList.push_back(std::move(moon));

    // Buttons
    zoomButtonIn = std::make_unique<Button>(
        0.85 * gv->getScreenWidth(),
        0.4 * gv->getScreenHeight(),
        200,
        50,
        SDL_Color{0, 128, 255, 255},
        "Zoom Out",
        gv->getRenderer(),
        gv->getFont(),
        std::bind(&GameController::onZoomButtonClickOut, this));
    buttonList.push_back(std::move(zoomButtonIn));

    zoomButtonOut = std::make_unique<Button>(
        0.85 * gv->getScreenWidth(),
        0.45 * gv->getScreenHeight(),
        200,
        50,
        SDL_Color{0, 78, 255, 255},
        "Zoom In",
        gv->getRenderer(),
        gv->getFont(),
        std::bind(&GameController::onZoomButtonClickIn, this));
    buttonList.push_back(std::move(zoomButtonOut));

    resetButton = std::make_unique<Button>( 
        0.85 * gv->getScreenWidth(),
        0.5 * gv->getScreenHeight(),
        200,
        50,
        SDL_Color{0, 78, 255, 255},
        "Reset Player",
        gv->getRenderer(),
        gv->getFont(),
        std::bind(&GameController::ResetPlayerButton, this));
    buttonList.push_back(std::move(resetButton));
}

void GameController::render(std::vector<Vector2D> futurePath)
{
    gv->render(entityList, buttonList, futurePath);
}

void GameController::gameLoop()
{
    int fps = 60;
    int updateTime = (int)((1 / fps) * 1000);
    Uint32 previousTime = SDL_GetTicks();
    while (gv->running())
    {
        long deltaTime = (SDL_GetTicks() - previousTime);

        if (deltaTime > updateTime)
        {
            previousTime = SDL_GetTicks();
            handleEvents();
            render(physicsSystem.update(entityList, deltaTime / 1000.0));
        }
    }
    gv->clean();
}

void GameController::handleEvents()
{
    inputHandler.handleInput(player, buttonList); 
    if (inputHandler.isQuit())
    {
        gv->setIsRunning(false);
    }
}