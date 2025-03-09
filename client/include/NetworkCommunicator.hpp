#ifndef NETWORKCOMMUNICATOR_HPP
#define NETWORKCOMMUNICATOR_HPP

#include <enet/enet.h>
#include <iostream>
#include <vector>

#include "GameController.hpp"
#include "NetworkPackets.hpp" 

class NetworkCommunicator
{
private:
    ENetAddress address;
    ENetEvent event;
    ENetPeer *serverPeer;
    ENetHost *client;

    std::string ipAddress;
    char username[32];
    int clientID;

public:
    NetworkCommunicator();
    ~NetworkCommunicator();

    void handleReceivedPacket();
    void disconnectFromServer();

    void sendInfoClientPacket(const char* username);
    void sendInputPacketToServer(std::vector<InputWithSequence>& inputList);

    int getClientID() { return clientID; }
    //GameStatePacket& getGameStatePacket() {return gameStatePacket; } 
};

#endif