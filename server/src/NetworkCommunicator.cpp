
#include "NetworkCommunicator.hpp"
#include "Player.hpp"
#include "RectangleCollider.hpp"
#include "Vector2D.hpp"

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
                                        Vector2D(70 + (50 * peerID), 500), 
                                        Vector2D(0, 0),
                                        100.0),
                                    username,
                                    peerID;
            player->setPlayerWidth(50);
            player->setPlayerHeight(100);
            entityList->push_back(std::move(player));

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

        auto client = clients.find(peerID); 
        if (client != clients.end())
        {
            
        }
        else
        {
            std::cout << "Unknown client!" << std::endl;
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
