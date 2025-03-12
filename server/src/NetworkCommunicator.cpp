
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
        std::vector<InputWithSequence> receivedInputs;

        const uint8_t *buffer = nullptr;
        size_t packetSize;
        PacketType type;

        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            handleConnections(event);
            break;
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
                sendNewConnectedPlayerPacket(event.peer->incomingPeerID, clientInfoPacket.username);
                std::cout << "Sent message that a new player has been added to all clients" << std::endl;
                ReceiveCliInfPacket(event.peer->incomingPeerID, clientInfoPacket);
                break;
            case PacketType::INPUT_PACKET:
                InputWithSequence::Deserialize(buffer, packetSize, receivedInputs);
                clientsInputBuffer.push_back({event.peer->incomingPeerID, receivedInputs});
                break;
            }

            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            handleDisconnections(event);
            break;
        }
    }
}

void NetworkCommunicator::handleConnections(ENetEvent &event)
{
    std::vector<uint32_t> playerIDs;

    for (std::pair<const enet_uint32, ENetPeer *> &client : clients)
    {
        playerIDs.push_back(client.first);
    }

    sendAllConnectedPlayersPacket(event.peer, playerIDs);

    clients[event.peer->incomingPeerID] = event.peer;
}

void NetworkCommunicator::handleDisconnections(ENetEvent &event)
{
    enet_uint16 peerID = event.peer->incomingPeerID;

    sendPlayerDisconnectedPacket(peerID);

    std::cout << "Sent message that a player has been removed to all clients" << std::endl;

    clients.erase(peerID);

    ReceiveDisPlaPacket(event.peer->incomingPeerID);

    std::cout << "Client disconnected! ID: " << peerID << std::endl;
}

void NetworkCommunicator::sendAllConnectedPlayersPacket(ENetPeer *peer, const std::vector<uint32_t> &playerIDs)
{
    ConnectedPlayersPacket packet(playerIDs);

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

    for (std::pair<const enet_uint32, ENetPeer *> &client : clients)
    {
        enet_peer_send(client.second, 0, enetPacket);
        enet_host_flush(server);
    }
}

void NetworkCommunicator::sendPlayerDisconnectedPacket(uint16_t &playerID)
{
    PlayerDisconnectedPacket packet(playerID);
    std::vector<uint8_t> data = packet.Serialize();
    ENetPacket *enetPacket = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);

    for (std::pair<const enet_uint32, ENetPeer *> &client : clients)
    {
        enet_peer_send(client.second, 0, enetPacket);
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