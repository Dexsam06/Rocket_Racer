
#include "../include/GameController.hpp"

GameController::GameController(NetworkCommunicator *nc, GameView *gv) : nc(nc), gv(gv)
{
    loadResources();

    nc->setConPlaCallback([this](ConnectedPlayersPacket & data) {
        this->HandleConPlaData(data);
    });

    nc->setNewPlaCallback([this](NewPlayerConnectedPacket & data) {
        this->HandleNewPlaData(data);
    });

    nc->setDisPlaCallback([this](PlayerDisconnectedPacket & data) {
        this->HandleDisPLaData(data);
    }); 
}

GameController::~GameController() {} 

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
            nc->handleReceivedPacket();
            clientPlayer->reconcileClientState(clientPlayer->getPosition(), Vector2D(nc->getGameStatePacket().clientState.serverPosX, nc->getGameStatePacket().clientState.serverPosY), nc->getGameStatePacket().clientState.rotation, 0.2f);
            removeConfirmedInputs();
            handleEvents();
            reapplyUnconfirmedInputs();
            physicsSystem.predictClientPosition(entityList, deltaTime / 1000.0);
            nc->sendInputPacketToServer(inputBuffer);
            interpolateOtherEntities();
            gv->render(entityList, buttonList);
        }
    }
    gv->clean();
}

void GameController::handleEvents()
{
    std::vector<KeyInput> keyInputPacket = inputHandler.handleInput(buttonList);

    if (keyInputPacket.empty())
    {
        return;
    }

    InputWithSequence inputWithSeq(inputSequenceNumber, keyInputPacket); 
    inputSequenceNumber++; 

    inputBuffer.push_back(inputWithSeq); 

    if (inputHandler.isQuit())
    {
        nc->disconnectFromServer(); 
        gv->setIsRunning(false);
    }
}
 
void GameController::removeConfirmedInputs() 
{
    int lastVerifiedInputID = nc->getGameStatePacket().clientState.lastVerifiedInputID;

    inputBuffer.erase(
        std::remove_if(inputBuffer.begin(), inputBuffer.end(),
                       [lastVerifiedInputID](const InputWithSequence &input)
                       {
                           return input.sequenceNumber <= lastVerifiedInputID;
                       }),
        inputBuffer.end());
}

void GameController::reapplyUnconfirmedInputs()
{
    for (InputWithSequence &InputPacket : inputBuffer) 
    {
        for (KeyInput &keyInput : InputPacket.keyInputPacket)
        {
            clientPlayer->applyInput(keyInput.keyCode, keyInput.duration);
        }
    }
}

void GameController::interpolateOtherEntities()
{
    uint32_t clientPlayerID = nc->getClientID();

    int entityIndex = 1;
    for (const auto &serverEntity : nc->getGameStatePacket().entities)
    {
        Vector2D clientEntityPosition = (entityList[entityIndex]->getPosition().x, entityList[entityIndex]->getPosition().y);
        Vector2D serverEntityPosition = (serverEntity.posX, serverEntity.posY);

        double deltaX = serverEntityPosition.x - clientEntityPosition.x;
        double deltaY = serverEntityPosition.y - clientEntityPosition.y;
        double distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);  

        double snapThreshold = 0.5;

        // Check if we should snap (if within the threshold) 
        if (distanceSquared < snapThreshold * snapThreshold)
        {
            entityList[entityIndex]->setPosition(Vector2D(serverEntityPosition.x, serverEntityPosition.y));
        }
        else
        {
            entityList[entityIndex]->setPosition(Vector2D(Physics::lerp(clientEntityPosition.x, serverEntity.posX, 0.2f), Physics::lerp(clientEntityPosition.y, serverEntity.posY, 0.2f)));
        }

        if (serverEntity.entityID < 1000)
        {
            double rotation = entityList[entityIndex]->getRotation();
            if (rotation > serverEntity.rotation - 5 && rotation < serverEntity.rotation + 5) 
            {
                rotation = serverEntity.rotation;
            }
            else
            {
                rotation = Physics::lerp(rotation, serverEntity.rotation, 0.2f);
            }
        }

        entityIndex++;
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

void GameController::HandleConPlaData(ConnectedPlayersPacket & data)
{
    int height, width; 

    SDL_Texture *playerTexture = textureManager.loadTexture("player", "../../res/player.png", gv->getRenderer());
    if (!playerTexture)
    {
        std::cerr << "Failed to load player texture!" << std::endl;
    }
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

    for (uint32_t& id : data.playerIDs)
    {
        std::unique_ptr<Player> player = std::make_unique<Player>(
            std::make_unique<RectangleCollider>(
                Vector2D(700 + (70 * id), 500),
                width,
                height),
            playerTexture,
            Vector2D(700 + (70 * id), 500),
            Vector2D(0, 0),
            playerMass,
            id);
        player->setPlayerWidth(width);
        player->setPlayerHeight(height);
    
        entityList.push_back(std::move(player));
    }

    std::cout << "Added all current connected players!" << std::endl;
}

void GameController::HandleNewPlaData(NewPlayerConnectedPacket & data)
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
            Vector2D(700 + (70 * data.playerID), 500),
            width,
            height),
        playerTexture,
        Vector2D(700 + (70 * data.playerID), 500),  
        Vector2D(0, 0),
        playerMass,
        data.playerID); 
    player->setPlayerWidth(width);
    player->setPlayerHeight(height);

    entityList.push_back(std::move(player));

    std::cout << "Added a new player with ID: " << data.playerID << std::endl;
}

void GameController::HandleDisPLaData(PlayerDisconnectedPacket & data)
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

    int height, width;

    SDL_Texture *playerTexture = textureManager.loadTexture("player", "../../res/player.png", gv->getRenderer());
    if (!playerTexture)
    {
        std::cerr << "Failed to load player texture!" << std::endl;
    }
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &width, &height);

    clientPlayer = std::make_unique<Player>(
        std::make_unique<RectangleCollider>(
            Vector2D(700 + (70 * nc->getClientID()), 500),
            width,
            height),
        playerTexture,
        Vector2D(700 + (70 * nc->getClientID()), 500),
        Vector2D(0, 0),
        playerMass,
        nc->getClientID());
    clientPlayer->setPlayerWidth(width);
    clientPlayer->setPlayerHeight(height);

    entityList.push_back(std::move(clientPlayer));

    std::cout << "clientPlayer added: " << nc->getClientID() << std::endl;

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