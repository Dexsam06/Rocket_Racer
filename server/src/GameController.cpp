
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
    }
}

void GameController::applyClientsInputs()
{
    for (auto it = nc->getClientsInputBuffer().begin(); it != nc->getClientsInputBuffer().end();)
    {
        NetworkCommunicator::ClientInputs &clientInputs = *it;
        int clientID = clientInputs.clientID;

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
                // Apply inputs to the player
                for (InputWithSequence &InputPacket : clientInputs.inputBuffer)
                {
                    for (KeyInput &keyInput : InputPacket.keyInputPacket)
                    {
                        clientPlayer->applyInput(keyInput.keyCode, keyInput.duration);
                    }
                    clientPlayer->setLastVerifiedInput(InputPacket.sequenceNumber);
                }
            }
        }
        else
        {
            std::cerr << "Player not found in the entity list when applying inputs for client ID " << clientID << std::endl;
        }

        it = nc->getClientsInputBuffer().erase(it);
    }
}

void GameController::sendGameStatePacketToClients()
{
    for (std::pair<enet_uint32, ENetPeer *> client : nc->getClientList())
    {
        GameStatePacket gameStatePacket;
        gameStatePacket.numEntities = entityList.size();

        int clientID = client.first;

        for (auto it = entityList.begin(); it != entityList.end();)
        {
            Player *player = dynamic_cast<Player *>(it->get());
            if (player && player->getID() == clientID)
            {
                gameStatePacket.clientState.lastVerifiedInputID = player->getLastVerifiedInput();
                gameStatePacket.clientState.serverPosX = player->getPosition().x;
                gameStatePacket.clientState.serverPosY = player->getPosition().y;
                gameStatePacket.clientState.rotation = player->getRotation();
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

        nc->sendGameStatePacketToClient(client.second, gameStatePacket);
    }
}

void GameController::HandleCliInfData(uint16_t &clientID, ClientInfoPacket &data)
{
    std::string usernameStr(data.username);
    std::unique_ptr<Player> player = std::make_unique<Player>(
        std::make_unique<RectangleCollider>(
            Vector2D(700 + (70 * clientID), 500),
            45,
            352),
        Vector2D(700 + (70 * clientID), 500),
        Vector2D(0, 0),
        100,
        clientID,
        usernameStr);
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
    std::cout << "Loading imaginary resources..." << std::endl;
}
