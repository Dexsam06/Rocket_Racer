
#include "../include/GameController.hpp"

#include "../include/NetworkCommunicator.hpp"

GameController::GameController(NetworkCommunicator *nc, GameView *gv) : nc(nc), gv(gv)
{
    nc->setConPlaCallback([this](ConnectedPlayersPacket &data)
                          { this->HandleConPlaData(data); });

    nc->setNewPlaCallback([this](NewPlayerConnectedPacket &data)
                          { this->HandleNewPlaData(data); });

    nc->setDisPlaCallback([this](PlayerDisconnectedPacket &data)
                          { this->HandleDisPLaData(data); });
}

GameController::~GameController() {}

void GameController::gameLoop()
{
    while (nc->getHasReceivedFirstGameState() == false && nc->getHasReceivedConnectedPlayers() == false)
    {
        nc->handleReceivedPacket();
        SDL_Delay(10);
    }

    std::cout << "Received necessary packets from the server!" << std::endl;

    loadResources();

    int fps = 60;
    int updateTime = (int)((1.0 / fps) * 1000); 
    Uint32 previousTime = SDL_GetTicks();

    std::cout << "Entering main game loop..." << std::endl;

    while (gv->running())
    {
        double deltaTime = (SDL_GetTicks() - previousTime);
        if (deltaTime > updateTime)
        {
            previousTime = SDL_GetTicks();
            nc->handleReceivedPacket();

            Vector2D serverPos(nc->getGameStatePacket().clientState.serverPosX, nc->getGameStatePacket().clientState.serverPosY);
            clientPlayer->reconcileClientState(serverPos, nc->getGameStatePacket().clientState.rotation, 0.2f);
        
            removeConfirmedInputs();  
            handleEvents();
            reapplyUnconfirmedInputs();
            physicsSystem.predictClientPosition(entityList, deltaTime / 1000.0);

            if (!inputBuffer.empty())
            {
                nc->sendInputPacketToServer(inputBuffer);

                for (auto &input : inputBuffer)
                {
                    unconfirmedInputs.push_back(input);
                }

                inputBuffer.clear();
            }

            interpolateOtherEntities();
            gv->render(entityList, buttonList);

            //std::cout << "ClientPlayer, id: " << nc->getClientID() << " has position x: " << clientPlayer->getPosition().x << std::endl;
        }
    }
    gv->clean();
}

void GameController::handleEvents()
{
    std::vector<KeyInput> keyInputPacket = inputHandler.handleInput(buttonList);

    if (!keyInputPacket.empty())
    {
        InputWithSequence inputWithSeq(inputSequenceNumber, keyInputPacket);
        inputSequenceNumber++;

        inputBuffer.push_back(inputWithSeq);
    }

    if (inputHandler.isQuit())
    {
        nc->disconnectFromServer();
        gv->setIsRunning(false);
    }
}

void GameController::removeConfirmedInputs()
{
    int lastVerifiedInputID = nc->getGameStatePacket().clientState.lastVerifiedInputID;

    unconfirmedInputs.erase(
        std::remove_if(unconfirmedInputs.begin(), unconfirmedInputs.end(),
                       [lastVerifiedInputID](const InputWithSequence &input)
                       {
                           return input.sequenceNumber <= lastVerifiedInputID;
                       }),
        unconfirmedInputs.end());
}

void GameController::reapplyUnconfirmedInputs()
{
    for (InputWithSequence &InputPacket : unconfirmedInputs)
    {
        for (KeyInput &keyInput : InputPacket.keyInputPacket)
        {
            clientPlayer->applyInput(keyInput.keyCode, keyInput.duration);
        }
    }
}

void GameController::interpolateOtherEntities() {
    for (const auto &serverEntity : nc->getGameStatePacket().entities) {
        // Skip the client's own entity (if present)
        if (serverEntity.entityID == nc->getClientID()) continue;

        // Find the corresponding entity in the client's list by ID
        for (auto &clientEntity : entityList) {
            if (clientEntity->getID() == serverEntity.entityID) {
                Vector2D serverPos(serverEntity.posX, serverEntity.posY);
                double serverRot = serverEntity.rotation;

                // Update position with interpolation/snapping
                Vector2D clientPos = clientEntity->getPosition();
                double deltaX = serverPos.x - clientPos.x;
                double deltaY = serverPos.y - clientPos.y;
                double distanceSq = deltaX * deltaX + deltaY * deltaY;

                if (distanceSq < 4.0) {
                    clientEntity->setPosition(serverPos);
                } else {
                    clientEntity->setPosition(Vector2D(Physics::lerp(clientPos.x, serverPos.x, 0.2f), Physics::lerp(clientPos.y, serverPos.y, 0.2f)));
                }

                // Update rotation
                double currentRot = clientEntity->getRotation();
                if (std::abs(currentRot - serverRot) < 5.0) {
                    clientEntity->setRotation(serverRot);
                } else {
                    clientEntity->setRotation(Physics::lerp(currentRot, serverRot, 0.2));
                }
                break; 
            }
        }
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

void GameController::HandleConPlaData(ConnectedPlayersPacket &data)
{
    int height, width;

    SDL_Texture *playerTexture = textureManager.loadTexture("player", "../../res/player.png", gv->getRenderer());
    if (!playerTexture)
    {
        std::cerr << "Failed to load player texture!" << std::endl;
    }
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

    for (uint32_t &id : data.playerIDs)
    {
        std::unique_ptr<Player> player = std::make_unique<Player>(
            std::make_unique<RectangleCollider>(
                Vector2D(700 + (70 * id), 500),
                width,
                height),
            playerTexture,
            Vector2D(700 + (70 * id), 500),
            Vector2D(0, 0),
            100.0,
            id);
        player->setPlayerWidth(width);
        player->setPlayerHeight(height);

        entityList.push_back(std::move(player));
    }

    std::cout << "Added all current connected players!" << std::endl;
}

void GameController::HandleNewPlaData(NewPlayerConnectedPacket &data)
{
    int height, width;

    SDL_Texture *playerTexture = textureManager.loadTexture("player", "../../res/player.png", gv->getRenderer());
    if (!playerTexture)
    {
        std::cerr << "Failed to load player texture!" << std::endl;
    }
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

    std::unique_ptr<Player> player = std::make_unique<Player>(
        std::make_unique<RectangleCollider>(
            Vector2D(760 + (100 * data.playerID), 500),
            width,
            height),
        playerTexture,
        Vector2D(760 + (100 * data.playerID), 500),
        Vector2D(0, 0),
        100.0,
        data.playerID);
    player->setPlayerWidth(width);
    player->setPlayerHeight(height);

    entityList.push_back(std::move(player));

    std::cout << "Added a new player with ID: " << data.playerID << std::endl;
}

void GameController::HandleDisPLaData(PlayerDisconnectedPacket &data)
{
    for (auto it = entityList.begin(); it != entityList.end();)
    {
        Player *player = dynamic_cast<Player *>(it->get()); 
        if (player && player->getID() == data.playerID)
        {
            it = entityList.erase(it);
            break;
        }
        else
        {
            ++it;
        }
    }

    std::cout << "Removed player with ID: " << data.playerID << std::endl;
}

void GameController::loadResources()
{
    // Background
    SDL_Texture *backgroundTexture = textureManager.loadTexture("background", "../../res/spaceBackgroundTiling.jpg", gv->getRenderer());
    gv->setBackground(backgroundTexture);

    // Planets
    SDL_Texture *moonTexture = textureManager.loadTexture("moon", "../../res/moon.png", gv->getRenderer());
    if(!moonTexture || moonTexture == nullptr) {
        std::cout << "Failed to load moon texture" << std::endl;
    }

    std::unique_ptr<Planet> moon = std::make_unique<Planet>(
        std::make_unique<CircleCollider>(
            Vector2D(960, (1080 / 2) + (352 / 2) - 1000), 
            400.0), 
        moonTexture,
        Vector2D(960, (1080 / 2) + (352 / 2) - 1000),    
        Vector2D(150, 0),
        100000.0,
        400.0,
        1001
        );
    entityList.insert(entityList.begin(), std::move(moon));  

    std::cout << "Added moon with id: " << 1001 << std::endl;

    SDL_Texture *earthTexture = textureManager.loadTexture("earth", "../../res/earth.png", gv->getRenderer());
    if(!earthTexture || earthTexture == nullptr) {
        std::cout << "Failed to load earth texture" << std::endl;
    }

    std::unique_ptr<Planet> earth = std::make_unique<Planet>(
        std::make_unique<CircleCollider>(
            Vector2D(960, (1080 / 2) + (352 / 2) + 1000), 
            1000.0), 
        earthTexture,
        Vector2D(960, (1080 / 2) + (352 / 2) + 1000),    
        Vector2D(0, 0),
        10000000.0,
        1000.0,
        1000
        );
    entityList.insert(entityList.begin(), std::move(earth));  

    std::cout << "Added earth with id: " << 1000 << std::endl;

    int height, width;

    SDL_Texture *playerTexture = textureManager.loadTexture("player", "../../res/player.png", gv->getRenderer());
    if (!playerTexture)
    {
        std::cerr << "Failed to load player texture!" << std::endl;
    }
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

    std::unique_ptr<Player> tempClientPlayer = std::make_unique<Player>(
        std::make_unique<RectangleCollider>(
            Vector2D(760 + (100 * nc->getClientID()), 500),
            width,
            height),
        playerTexture,
        Vector2D(760 + (100 * nc->getClientID()), 500),
        Vector2D(0, 0),
        100.0,
        nc->getClientID());
    tempClientPlayer->setPlayerWidth(width); 
    tempClientPlayer->setPlayerHeight(height);

    clientPlayer = tempClientPlayer.get();

    entityList.insert(entityList.begin(), std::move(tempClientPlayer));

    std::cout << "clientPlayer added, id: " << nc->getClientID() << std::endl;

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