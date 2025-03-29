
#include "../include/GameController.hpp"

GameController::GameController(NetworkCommunicator *nc) : nc(nc)
{
    loadResources();

    nc->setCliInfCallback([this](uint16_t clientID, ClientInfoPacket &data)
                          { this->HandleCliInfData(clientID, data); });

    nc->setDisPlaCallback([this](uint16_t clientID)
                          { this->HandleDisPlaData(clientID); });
}

GameController::~GameController() {}

void GameController::gameLoop()
{
    int fps = 12;
    int updateTime = (int)((1.0 / fps) * 1000); // Milliseconds per frame
    auto previousTime = std::chrono::steady_clock::now();

    while (nc->serverRunning())
    {
        auto currentTime = std::chrono::steady_clock::now();
        long deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();

        if (deltaTime > updateTime)
        {
            previousTime = std::chrono::steady_clock::now();
            nc->NetworkHandler();
            applyClientsInputs();
            physicsSystem.update(entityList, deltaTime / 1000.0);
            sendGameStatePacketToClients();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void GameController::applyClientsInputs()
{
    auto &clientsInputBuffer = nc->getClientsInputBuffer();

    while (!clientsInputBuffer.empty()) 
    {
        auto clientInput = clientsInputBuffer.begin()->second;
        int clientID = clientsInputBuffer.begin()->first;

        auto playerIt = std::find_if(entityList.begin(), entityList.end(),
                                     [clientID](const std::unique_ptr<Entity> &entity)
                                     {
                                         // Attempt to cast the entity to Player and check if the cast was successful and the ID matches
                                         Player *player = dynamic_cast<Player *>(entity.get());
                                         return (player != nullptr && player->getID() == clientID);
                                     });

        if (playerIt != entityList.end())
        {
            // Successfully found the player in the entity list and cast it
            Player *clientPlayer = dynamic_cast<Player *>(playerIt->get());
            if (clientPlayer)
            {
                switch (clientInput.key) {
                    case SDLK_LEFT:  // Use SDL key codes directly
                        clientPlayer->setisTurningLeft(clientInput.pressed);
                        break;
                    case SDLK_RIGHT:
                        clientPlayer->setisTurningRight(clientInput.pressed);
                        break;
                    case SDLK_SPACE:
                        clientPlayer->setisThrusting(clientInput.pressed);
                        break;
                } 
            }
        }
        else 
        {
            std::cerr << "Player not found in the entity list when applying inputs for client ID " << clientID << std::endl;
        }

        clientsInputBuffer.erase(clientID); 
    }
}

void GameController::sendGameStatePacketToClients()
{
    for (std::pair<enet_uint32, std::pair<std::string, ENetPeer *>> client : nc->getClientList())
    {
        GameStatePacket gameStatePacket;
        gameStatePacket.numEntities = entityList.size() - 1;

        int clientID = client.first;

        for (auto it = entityList.begin(); it != entityList.end();)
        {
            Player *player = dynamic_cast<Player *>(it->get());
            if (player && player->getID() == clientID)
            {
                gameStatePacket.clientState.serverPosX = player->getPosition().x;
                gameStatePacket.clientState.serverPosY = player->getPosition().y;
                gameStatePacket.clientState.rotation = player->getRotation();
                gameStatePacket.clientState.isThrusting = player->getisThrusting(); 
                gameStatePacket.clientState.isTurningLeft = player->getisTurningLeft();
                gameStatePacket.clientState.isTurningRight = player->getisTurningRight();
                break; 
            }
            else
            {
                ++it; 
            }
        }

        for (const std::unique_ptr<Entity> &entity : entityList)
        {
            if (entity->getID() == clientID)
            {
                continue;
            }

            EntityState entityState;
            entityState.entityID = entity->getID();
            entityState.posX = entity->getPosition().x;
            entityState.posY = entity->getPosition().y;
            entityState.rotation = entity->getRotation();
            gameStatePacket.entities.push_back(entityState);
        }

        nc->sendGameStatePacketToClient(client.second.second, gameStatePacket);
    }
}

void GameController::HandleCliInfData(uint16_t &clientID, ClientInfoPacket &data)
{
    std::unique_ptr<Player> player = std::make_unique<Player>(
        std::make_unique<RectangleCollider>(
            Vector2D(760 + (200 * clientID), 500),
            Vector2D(45, 352)), 
        Vector2D(760 + (200 * clientID), 500),
        Vector2D(0, 0), 
        100,
        clientID);
    player->setPlayerWidth(45);
    player->setPlayerHeight(352);

    entityList.push_back(std::move(player));

    std::cout << "Added a new player with ID: " << clientID << std::endl;
}

void GameController::HandleDisPlaData(uint16_t &clientID)
{
    for (auto it = entityList.begin(); it != entityList.end();)
    {
        Player *player = dynamic_cast<Player *>(it->get());
        if (player && player->getID() == clientID)
        {
            it = entityList.erase(it);
            break;
        }
        else
        {
            ++it;
        }
    }
}

void GameController::loadResources()
{
    std::cout << "Loading resources..." << std::endl;

    // Planets
    std::unique_ptr<Planet> earth = std::make_unique<Planet>(
        std::make_unique<CircleCollider>(
            Vector2D(960, (1080 / 2) + (352 / 2 + 2000)),
            2000.0),
        Vector2D(960, (1080 / 2) + (352 / 2 + 2000)),
        Vector2D(0, 0),
        100000000.0,
        2000.0,
        1000);
    entityList.push_back(std::move(earth));

    std::cout << "Added earth with id: " << 1000 << std::endl;

    std::unique_ptr<Planet> moon = std::make_unique<Planet>(
        std::make_unique<CircleCollider>(
            Vector2D(1300, (1080 / 2) + (352 / 2) - 3000),
            800.0),
        Vector2D(1300, (1080 / 2) + (352 / 2) - 3000), 
        Vector2D(300, 0),
        1000000.0,
        800.0,
        1001);
    entityList.push_back(std::move(moon)); 

    std::cout << "Added moon with id: " << 1001 << std::endl; 
}
