#include "../include/NetworkCommunicator.hpp"

NetworkCommunicator::NetworkCommunicator()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An Error has occured while initializing enet\n");
        return;
    }
    atexit(enet_deinitialize);

    client = enet_host_create(NULL, 1, 1, 0, 0);

    if (client == NULL)
    {
        fprintf(stderr, "An Error occured while trying to create a ENet client host! \n");
        return;
    }

    std::cout << "Skriv ip-address: " << std::endl;
    ipAddress = "127.0.0.1";
    std::cout << "Type your username: ";
    std::cin >> username;  

    enet_address_set_host(&address, ipAddress.c_str());
    address.port = 7777; 

    serverPeer = enet_host_connect(client, &address, 1, 0);
    if (serverPeer == NULL)
    {
        fprintf(stderr, "No available peers for initalizing an ENet connection!\n");
        return;
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        clientID = serverPeer->incomingPeerID;
        std::cout << "Connection to " << address.host << ":7777 succeeded!" << std::endl;

        sendInfoClientPacket(username); 
        std::cout << "Sent client info to server!" << std::endl;
    }
    else
    {
        enet_peer_reset(serverPeer);
        std::cout << "Connection to " << address.host << ":7777 failed!" << std::endl;
        return;
    }
}

void NetworkCommunicator::handleReceivedPacket() 
{
    while (enet_host_service(client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            if (!event.packet || event.packet->data == nullptr)
            {
                std::cerr << "Error: Null or corrupted packet received!" << std::endl;
                return;
            }

            const uint8_t* buffer = event.packet->data; 
            size_t packetSize = event.packet->dataLength;

            PacketType type = static_cast<PacketType>(buffer[0]);

            ConnectedPlayersPacket connectedPlayersPacket; 
            NewPlayerConnectedPacket newPlayerPacket;
            PlayerDisconnectedPacket playerDisPacket;

            switch (type)
            {
                case PacketType::CONNECTED_PLAYERS_PACKET: 

                    connectedPlayersPacket.Deserialize(buffer, packetSize);
                    std::cout << "Received all connected players from server" << std::endl; 
                    ReceiveConPlaPacket(connectedPlayersPacket);
                    break;
                case PacketType::NEW_PLAYER_CONNECTED_PACKET:
                    newPlayerPacket.Deserialize(buffer, packetSize);
                    std::cout << "New player added, id: " << newPlayerPacket.playerID << std::endl; 
                    ReceiveNewPlaPacket(newPlayerPacket); 
                    break;
                case PacketType::PLAYER_DISCONNECTED_PACKET: 
                    playerDisPacket.Deserialize(buffer, packetSize);
                    std::cout << "Player disconnected, id: " << playerDisPacket.playerID << std::endl;
                    ReceiveDisPlaPacket(playerDisPacket); 
                    break;
                case PacketType::GAME_STATE_PACKET:
                    gameStatePacket.Deserialize(buffer, packetSize); 
            } 
        }
    }
}

void NetworkCommunicator::disconnectFromServer()
{
    enet_peer_disconnect(serverPeer, 0);

    while (enet_host_service(client, &event, 3000) > 0) 
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            puts("Disconnection succeeded!");
            break;
        }
    }
}

void NetworkCommunicator::sendInfoClientPacket(const char* username)
{
    ClientInfoPacket packet(username);

    std::vector<uint8_t> data = packet.Serialize();

    ENetPacket* enetPacket = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(serverPeer, 0, enetPacket); 
    enet_host_flush(client);
}

void NetworkCommunicator::sendInputPacketToServer(std::vector<InputWithSequence>& inputList)
{
    std::vector<uint8_t> data = InputWithSequence::Serialize(inputList);

    ENetPacket* packet = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(serverPeer, 0, packet);

    enet_host_flush(client);  
}