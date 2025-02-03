
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

NetworkCommunicator::~NetworkCommunicator(){} 

void NetworkCommunicator::NetworkHandler()
{
    while (enet_host_service(server, &event, 1000) > 0)
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
        if (enet_host_service(event.peer->host, &clientEvent, 0) > 0) { 
            if (clientEvent.type == ENET_EVENT_TYPE_RECEIVE) {
                // Handle the data sent by the client
                struct ClientInfo {
                    char username[32];
                };

                ClientInfo* clientData = reinterpret_cast<ClientInfo*>(clientEvent.packet->data);

                 std::cout << "Client connected! Username: " << clientData->username << " and ID: " << peerID << std::endl;
                 clients[peerID] = {event.peer, clientData->username};

                enet_packet_destroy(clientEvent.packet);
            }
        }
}

void NetworkCommunicator::handleIncomingPacket(ENetEvent &event)
{
    enet_uint32 peerID = event.peer->incomingPeerID;

    if (event.packet && event.packet->dataLength == sizeof(ClientData))
    {
        ClientData *receivedClient = reinterpret_cast<ClientData *>(event.packet->data);

        auto client = clients.find(peerID); 
        if (client != clients.end()) 
        {
            client->second = *receivedClient;  
            client->second.peer = event.peer; 
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

    std::cout << "Client disconnected! Peer ID: " << peerID << std::endl; 

    clients.erase(peerID);
}
