
#include "../include/GameController.hpp"

GameController::GameController(NetworkCommunicator *nc) : nc(nc)
{
    loadResources();
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
    for (NetworkCommunicator::ClientInputs &clientInputs : nc->getClientsInputBuffer())
    {
        int clientID = clientInputs.clientID;

        auto it = std::find_if(entityList.begin(), entityList.end(),
                               [clientID](const std::unique_ptr<Entity> &entity)
                               {
                                   // Attempt to cast the entity to Player and check if the cast was successful and the ID matches
                                   Player *player = dynamic_cast<Player *>(entity.get());
                                   return (player != nullptr && player->getID() == clientID);
                               });

        if (it != entityList.end())
        {
            // Successfully found the player in the entity list and cast it
            Player *clientPlayer = dynamic_cast<Player *>(it->get());
            if (clientPlayer)
            {
                // Apply inputs to the player
                for (InputWithSequence &InputPacket : clientInputs.inputBuffer)
                {
                    for (KeyInput &keyInput : InputPacket.keyInputPacket)
                    {
                        clientPlayer->applyInput(keyInput.keyCode, keyInput.duration);
                    }
                }
            }
        }
        else
        {
            std::cerr << "Player not found in the entity list when applying inputs for client ID " << clientID << std::endl;
        }
    }
}

void GameController::sendGameStatePacketToClients()
{
    for (std::pair<enet_uint32, ENetPeer *> client : nc->getClientList())
    {
        GameStatePacket gameStatePacket;
        gameStatePacket.numEntities = entityList.size();

        int clientID = client.first;
        auto it = std::find_if(entityList.begin(), entityList.end(),
                               [clientID](const std::unique_ptr<Entity> &entity)
                               {
                                   // Attempt to cast the entity to Player and check if the cast was successful and the ID matches
                                   Player *player = dynamic_cast<Player *>(entity.get());
                                   return (player != nullptr && player->getID() == clientID);
                               });

        if (it != entityList.end())
        {
            // Successfully found the player in the entity list and cast it
            Player *clientPlayer = dynamic_cast<Player *>(it->get());
            if (clientPlayer) 
            {
                gameStatePacket.clientState.lastVerifiedInputID = clientPlayer->getLastVerifiedInput();
                gameStatePacket.clientState.rotation = clientPlayer->getRotation();
                gameStatePacket.clientState.serverPosX = clientPlayer->getPosition().x;
                gameStatePacket.clientState.serverPosY = clientPlayer->getPosition().y;
            }
        }
        else
        {
            std::cerr << "Player not found in the entity list when creating clientState for client ID " << clientID << std::endl;
        }
        for (const std::unique_ptr<Entity>& entity : entityList)
        {
            if (entity->getID() == client.first)
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

void GameController::loadResources() 
{
    std::cout << "Loading imaginary resources..." << std::endl;
}
