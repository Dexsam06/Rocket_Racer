
#include "../include/NetworkCommunicator.hpp"
#include <string.h>

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

void NetworkCommunicator::NetworkHandler()
{
    while (enet_host_service(server, &event, 0) > 0)
    {
        ClientInfoPacket clientInfoPacket;
        ClientInputPacket clientInputPacket; 

        const uint8_t *buffer = nullptr;
        size_t packetSize;
        PacketType type;

        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:

            if (!event.packet || event.packet->data == nullptr)
            {
                std::cerr << "Error: Null or corrupted packet received!" << std::endl;
                return;
            }

            buffer = event.packet->data;
            packetSize = event.packet->dataLength;

            type = static_cast<PacketType>(buffer[0]);

            switch (type)
            {
            case PacketType::CLIENT_INFO_PACKET:
                clientInfoPacket.Deserialize(buffer, packetSize);
                std::cout << "Received client info from ID: " << event.peer->incomingPeerID << std::endl;
                handleConnections(event, clientInfoPacket);
                break;
            case PacketType::INPUT_PACKET:
                clientInputPacket.Deserialize(buffer, packetSize);
                clientsInputBuffer[event.peer->incomingPeerID] = clientInputPacket;
                break;
            }

            break;
        case ENET_EVENT_TYPE_DISCONNECT: 
            handleDisconnections(event);
            break;
        }

        enet_packet_destroy(event.packet);
    }
}

void NetworkCommunicator::handleConnections(ENetEvent &event, ClientInfoPacket &clientInfoPacket)
{
    std::vector<std::pair<enet_uint32, std::string>> connectedClients;

    for (std::pair<const enet_uint32, std::pair<std::string, ENetPeer * >>& client : clients)
    {
        connectedClients.push_back({client.first, client.second.first});
    }

    sendAllConnectedPlayersPacket(event.peer, connectedClients);
    std::cout << "Sent all connected players to the newly connected client with id: " << event.peer->incomingPeerID << std::endl;

    sendNewConnectedPlayerPacket(event.peer->incomingPeerID, clientInfoPacket.username);
    std::cout << "Sent message that a new player has been added to all clients" << std::endl;
    ReceiveCliInfPacket(event.peer->incomingPeerID, clientInfoPacket);

    clients[event.peer->incomingPeerID] = {clientInfoPacket.username, event.peer};
}

void NetworkCommunicator::handleDisconnections(ENetEvent &event)
{
    enet_uint16 peerID = event.peer->incomingPeerID;

    clients.erase(peerID);

    ReceiveDisPlaPacket(event.peer->incomingPeerID);

    std::cout << "Client disconnected! ID: " << peerID << std::endl;

    sendPlayerDisconnectedPacket(peerID);

    std::cout << "Sent message that a player has been removed to all clients" << std::endl;
}

void NetworkCommunicator::sendAllConnectedPlayersPacket(ENetPeer *peer, const std::vector<std::pair<uint32_t, std::string>> &clientsConnected)
{
    ConnectedPlayersPacket packet(clientsConnected, peer->incomingPeerID);

    std::vector<uint8_t> data = packet.Serialize();
    ENetPacket *enetPacket = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(peer, 0, enetPacket);
    enet_host_flush(server);
}

void NetworkCommunicator::sendNewConnectedPlayerPacket(uint16_t &playerID, char username[32])
{
    NewPlayerConnectedPacket packet(playerID, username);
    std::vector<uint8_t> data = packet.Serialize();
    ENetPacket *enetPacket = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);

    for (std::pair<const enet_uint32, std::pair<std::string, ENetPeer *>> &client : clients)
    {
        enet_peer_send(client.second.second, 0, enetPacket);
        enet_host_flush(server);
    }
}

void NetworkCommunicator::sendPlayerDisconnectedPacket(uint16_t &playerID)
{
    PlayerDisconnectedPacket packet(playerID);
    std::vector<uint8_t> data = packet.Serialize();
    ENetPacket *enetPacket = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);

    for (std::pair<const enet_uint32, std::pair<std::string, ENetPeer *>> &client : clients)
    {
        enet_peer_send(client.second.second, 0, enetPacket);
        enet_host_flush(server);
    }
}

void NetworkCommunicator::sendGameStatePacketToClient(ENetPeer *peer, const GameStatePacket &packet)
{
    std::vector<uint8_t> data = packet.Serialize();
    ENetPacket *enetPacket = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(peer, 0, enetPacket);
    enet_host_flush(server);
}