
#ifndef NETWORKCOMMUNICATOR_HPP
#define NETWORKCOMMUNICATOR_HPP

#include <iostream>
#include <vector>
#include <enet/enet.h>
#include <cstring>
#include <bits/this_thread_sleep.h>
#include <signal.h>

class NetworkCommunicator
{

public:
    NetworkCommunicator(std::string &ipAddress); 
    ~NetworkCommunicator(); 
    void NetworkHandler();

private:
    ENetAddress address;
    ENetEvent event;
    ENetPeer *serverPeer;  
    ENetHost *client;

    std::string username; 

    static bool connected; 

    static void onExit(int signum) {connected = false; } 
};

#endif