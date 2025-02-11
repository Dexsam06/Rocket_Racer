
#ifndef NETWORKCOMMUNICATOR_HPP
#define NETWORKCOMMUNICATOR_HPP

#include <iostream>
#include <vector>
#include <enet/enet.h>
#include <cstring>
#include <thread>
#include <signal.h>
#include <functional>

#include "Entity.hpp"

class NetworkCommunicator
{

public:
    NetworkCommunicator(std::string &ipAddress);
    ~NetworkCommunicator();
    void NetworkHandler();

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

    struct PlayerInputs
    {
        double thrust;
        double rotation;
    };

    void setEntityCallback(std::function<void(std::vector<EntityData>)> callback) { entityCallback = callback; }
    void sendInputToServer(PlayerInputs inputs);

private:
    ENetAddress address;
    ENetEvent event;
    ENetPeer *serverPeer;
    ENetHost *client;

    std::function<void(std::vector<EntityData>)> entityCallback; 

    struct ConnectData
    {
        std::string username;
        int width;
        int height;
    };
    ConnectData connectData = {"", 45, 352}; 

    static bool connected;

    static void onExit(int signum) { connected = false; }
};

#endif