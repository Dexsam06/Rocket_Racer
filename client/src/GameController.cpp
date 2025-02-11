
#include "GameController.hpp"
#include "Physics.hpp"
#include <iostream>

GameController::GameController(GameView *gv, NetworkCommunicator *nc) : gv(gv), nc(nc)
{
    loadResources();

    // Register the callback function
    nc->setEntityCallback([this](std::vector<NetworkCommunicator::EntityData> data)
                          { handleIncomingEntityData(data); });
}

GameController::~GameController() {}

void GameController::loadResources()
{
    // Background
    SDL_Texture *backgroundTexture = textureManager.loadTexture("background", "../../res/spaceBackgroundTiling.jpg", gv->getRenderer());
    gv->setBackground(backgroundTexture);

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
        nc->NetworkHandler();
        if (deltaTime > updateTime)
        {
            previousTime = SDL_GetTicks();
            handleEvents();
            nc->NetworkHandler();
            render(physicsSystem.update(entityList, deltaTime / 1000.0));
        }
    }
    gv->clean();
}

void GameController::handleEvents()
{
    InputHandler::PlayerInputs inputs = inputHandler.handleInput(dynamic_cast<Player *>(entityList[0].get()), buttonList);
    std::cout << "maybe it be crashing here" << std::endl;
    NetworkCommunicator::PlayerInputs playerInputs;
    playerInputs.thrust = inputs.thrust;
    playerInputs.rotation = inputs.rotation;

    nc->sendInputToServer(playerInputs);

    if (inputHandler.isQuit())
    {
        gv->setIsRunning(false);
    }
}

void GameController::onZoomButtonClickOut()
{
    gv->setScalingFactors(Vector2D(0.9, 0.9));
}
void GameController::onZoomButtonClickIn()
{
    gv->setScalingFactors(Vector2D(1.1, 1.1));
}

void GameController::handleIncomingEntityData(std::vector<NetworkCommunicator::EntityData> entityDataList)
{
    for (auto &data : entityDataList)
    {
        bool found = false;

        for (auto &entity : entityList)
        {
            Player *player = dynamic_cast<Player *>(entity.get());
            Planet *planet = dynamic_cast<Planet *>(entity.get());

            // Handle player entity
            if (data.entityType == 0 && player && player->getPeerID() == data.ID)
            {
                player->setPeerID(data.ID);
                player->setPosition(Vector2D(data.posX, data.posY));
                player->setVelocity(Vector2D(data.velocityX, data.velocityY));
                player->setAcceleration(Vector2D(data.accelerationX, data.accelerationY));
                player->setMass(data.mass);

                found = true;
                break;
            }

            // Handle planet entity
            else if (data.entityType == 1 && planet && planet->getUniqueID() == data.ID)
            {
                planet->setUniqueID(data.ID);
                planet->setPosition(Vector2D(data.posX, data.posY));
                planet->setVelocity(Vector2D(data.velocityX, data.velocityY));
                planet->setAcceleration(Vector2D(data.accelerationX, data.accelerationY));
                found = true;
                break;
            }
        }

        if (!found)
        {
            if (data.entityType == 0)
            {
                int height, width;
                // Player
                SDL_Texture *playerTexture = textureManager.loadTexture("player", "../../res/player.png", gv->getRenderer());
                if (!playerTexture)
                {
                    std::cerr << "Failed to load player texture!" << std::endl;
                }
                SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

                player = std::make_unique<Player>(
                    std::make_unique<RectangleCollider>(
                        Vector2D(data.posX, data.posY),
                        width,
                        height,
                        0),
                    playerTexture,
                    Vector2D(data.posX, data.posY),
                    Vector2D(data.velocityX, data.velocityY),
                    data.mass,
                    data.ID);
                player->setPlayerWidth(width);
                player->setPlayerHeight(height);

                entityList.push_back(std::move(player));

                std::cout << "New player added with peerID: " << data.ID << std::endl;
            }

            else if (data.entityType == 1)
            {
                std::string filePath = "../../res/planet" + std::to_string(data.ID) + ".png";
                SDL_Texture *texture = textureManager.loadTexture("planet", filePath, gv->getRenderer());

                std::unique_ptr<Planet> planet = std::make_unique<Planet>(
                    std::make_unique<CircleCollider>(
                        Vector2D(data.posX, data.posY),
                        data.radius),
                    texture,
                    Vector2D(data.posX, data.posY),
                    Vector2D(data.velocityX, data.velocityY),
                    data.mass,
                    data.radius,
                    data.ID);
                entityList.push_back(std::move(planet));

                std::cout << "New planet added with uniqueID: " << data.ID << std::endl;
            }
        }
    }
}