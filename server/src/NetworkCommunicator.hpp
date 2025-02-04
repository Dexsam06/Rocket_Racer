
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
    bool serverRunning() { return serverRunningState; }
    void setEntityList(std::vector<std::unique_ptr<Entity>> *entityList) { this->entityList = entityList; }

private:
    ENetAddress address;
    ENetEvent event;
    ENetHost *server;

    bool serverRunningState = true;

    std::vector<std::unique_ptr<Entity>> *entityList;
    std::unordered_map<enet_uint32, ENetPeer *> clients;

    struct ClientConnectData
    {
        std::string username;
        int width;
        int height;
    };

    struct ClientGameData
    {
        double thrust;
        double rotation;
    };

    void handleIncomingConnections(ENetEvent &event);
    void handleDisconnections(ENetEvent &event);
    void handleIncomingPacket(ENetEvent &event);
};

#endif