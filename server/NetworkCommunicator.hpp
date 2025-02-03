
#ifndef NETWORKCOMMUNICATOR_HPP
#define NETWORKCOMMUNICATOR_HPP

#include <iostream>
#include <unordered_map>
#include <enet/enet.h>

class NetworkCommunicator
{

public:
    NetworkCommunicator();
    ~NetworkCommunicator();
    void NetworkHandler();

private:
    ENetAddress address;
    ENetEvent event;
    ENetHost *server; 

    struct ClientData
    {
        ENetPeer *peer;
        std::string username;
    };
    std::unordered_map<enet_uint32, ClientData> clients;

    void handleIncomingConnections(ENetEvent& event); 
    void handleDisconnections(ENetEvent& event);
    void handleIncomingPacket(ENetEvent& event);
};

#endif