
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
    

    struct EntityData
    {
        int entityType; // 0 = Player, 1 = Planet
        enet_uint32 ID; 
        double radius;
        double posX, posY;
        double velocityX, velocityY;
        double accelerationX, accelerationY;
        double mass;
    };

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
    }; ConnectData connectData = {"", 45, 352};

    static bool connected;

    static void onExit(int signum) { connected = false; }
};

#endif