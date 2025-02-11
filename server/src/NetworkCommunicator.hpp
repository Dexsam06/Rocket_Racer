
#ifndef NETWORKCOMMUNICATOR_HPP
#define NETWORKCOMMUNICATOR_HPP

#include <iostream>
#include <unordered_map>
#include <enet/enet.h>

#include "Player.hpp"
#include "RectangleCollider.hpp"
#include "Vector2D.hpp"
#include "Planet.hpp"

class NetworkCommunicator
{

public:
    NetworkCommunicator();
    ~NetworkCommunicator();
    void NetworkHandler();
    bool serverRunning() { return serverRunningState; }
    void setEntityList(std::vector<std::unique_ptr<Entity>> *entityList) { this->entityList = entityList; }
    void sendWorldStateToClients();

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

#pragma pack(push, 1)
    struct EntityData
    {
        enet_uint32 entityType;
        enet_uint32 ID;
        float posX, posY;
        float velocityX, velocityY;
        float accelerationX, accelerationY;
        float mass;
        float radius; 
    };
#pragma pack(pop)

    void handleDisconnections(ENetEvent &event);
    void handleIncomingPacket(ENetEvent &event);
    ENetPacket *createWorldStatePacket(enet_uint32 peerID);
};

#endif