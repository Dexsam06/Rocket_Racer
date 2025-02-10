
#include "NetworkCommunicator.hpp"

NetworkCommunicator::NetworkCommunicator()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An Error has occured while initializing enet\n");
        return;
    }
    atexit(enet_deinitialize);

    address.host = ENET_HOST_ANY;
    address.port = 7777;

    server = enet_host_create(&address, 16, 1, 0, 0);

    if (server == NULL)
    {
        fprintf(stderr, "An Error occured while trying to create a ENet server! \n");
        return;
    }
}

NetworkCommunicator::~NetworkCommunicator() {}

void NetworkCommunicator::NetworkHandler()
{
    while (enet_host_service(server, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            handleIncomingConnections(event);
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            handleIncomingPacket(event);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            handleDisconnections(event);
            break;
        }
    }
}

void NetworkCommunicator::handleIncomingConnections(ENetEvent &event)
{
    enet_uint32 peerID = event.peer->incomingPeerID;

    ENetEvent clientEvent;
    if (enet_host_service(event.peer->host, &clientEvent, 0) > 0)
    {
        if (clientEvent.type == ENET_EVENT_TYPE_RECEIVE)
        {

            ClientConnectData *client = reinterpret_cast<ClientConnectData *>(clientEvent.packet->data);
            clients[peerID] = event.peer;
            std::cout << "Client connected! Username: " << client->username << " and ID: " << peerID << std::endl;

            std::unique_ptr<Player> player = std::make_unique<Player>(
                std::make_unique<RectangleCollider>(
                    Vector2D(700 + (50 * peerID), 500),
                    client->width,
                    client->height),
                Vector2D(700 + (50 * peerID), 500),
                Vector2D(0, 0),
                100.0,
                client->username,
                peerID);
            player->setPlayerWidth(50);
            player->setPlayerHeight(100);
            entityList->push_back(std::move(player));

            ENetPacket *packet = createWorldStatePacket(player->getPeerID());
            enet_peer_send(event.peer, 0, packet);
            enet_packet_destroy(clientEvent.packet);
        }
    }
}

void NetworkCommunicator::handleIncomingPacket(ENetEvent &event)
{
    enet_uint32 peerID = event.peer->incomingPeerID;

    if (event.packet && event.packet->dataLength == sizeof(ClientGameData))
    {
        ClientGameData *gameData = reinterpret_cast<ClientGameData *>(event.packet->data);

        for (auto it = entityList->begin(); it != entityList->end();)
        {
            Player *player = dynamic_cast<Player *>(it->get());
            if (player && player->getPeerID() == peerID)
            {
                player->setThrust(gameData->thrust);
                player->setRotation(gameData->rotation);
                break;
            }
            else
            {
                ++it;
            }
        }
    }
    enet_packet_destroy(event.packet);
}

void NetworkCommunicator::handleDisconnections(ENetEvent &event)
{
    enet_uint32 peerID = event.peer->incomingPeerID;

    clients.erase(peerID);

    for (auto it = entityList->begin(); it != entityList->end();)
    {
        Player *player = dynamic_cast<Player *>(it->get());
        if (player && player->getPeerID() == peerID)
        {
            it = entityList->erase(it);
            break;
        }
        else
        {
            ++it;
        }
    }

    std::cout << "Client disconnected! Peer ID: " << peerID << std::endl;
}

void NetworkCommunicator::sendWorldStateToClients()
{
    for (const auto &client : clients)
    {
        ENetPacket *packet = createWorldStatePacket(client.first);
        enet_peer_send(client.second, 0, packet);
        enet_packet_destroy(packet);
    }
}

ENetPacket *NetworkCommunicator::createWorldStatePacket(enet_uint32 peerID)
{
    std::vector<EntityData> dataList;

    for (const auto &entity : *entityList)
    {
        EntityData data;

        if (Player *player = dynamic_cast<Player *>(entity.get()))
        {
            data.entityType = 0; // Player
            data.ID = player->getPeerID();
        }
        else if (Planet *planet = dynamic_cast<Planet *>(entity.get()))
        {
            data.entityType = 1; // Planet
            data.radius = planet->getRadius();
            data.ID = planet->getUniqueID();
        }

        // Serialize common attributes
        data.posX = entity->getPosition().x;
        data.posY = entity->getPosition().y;
        data.velocityX = entity->getVelocity().x;
        data.velocityY = entity->getVelocity().y;
        data.accelerationX = entity->getAcceleration().x;
        data.accelerationY = entity->getAcceleration().y;
        data.mass = entity->getMass();

        dataList.push_back(data);
    }

    size_t dataSize = dataList.size() * sizeof(EntityData);
    ENetPacket *packet = enet_packet_create(dataList.data(), dataSize, ENET_PACKET_FLAG_RELIABLE);

    return packet;
}