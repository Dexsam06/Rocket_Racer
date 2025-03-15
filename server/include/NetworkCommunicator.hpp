
#ifndef NETWORKCOMMUNICATOR_HPP
#define NETWORKCOMMUNICATOR_HPP

#include <iostream>
#include <unordered_map>
#include <enet/enet.h>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <functional>

#include "NetworkPackets.hpp"

class NetworkCommunicator
{

public:
    NetworkCommunicator();
    ~NetworkCommunicator() {}
    void NetworkHandler();
    bool serverRunning() { return serverRunningState; }
    void sendGameStatePacketToClient(ENetPeer* peer, const GameStatePacket& gameStatePacket); 

    // Input information
    struct ClientInputs
    {
        int clientID;
        std::vector<InputWithSequence> inputBuffer;
    };

    std::vector<ClientInputs>& getClientsInputBuffer() { return clientsInputBuffer; }
    std::unordered_map<enet_uint32, ENetPeer *>& getClientList() {return clients; }

    using CliInfCallback = std::function<void(uint16_t, ClientInfoPacket &)>;
    using DisPlaCallback = std::function<void(uint16_t)>;

    void setCliInfCallback(CliInfCallback callback) {cliInfCallback = callback; }
    void setDisPlaCallback(DisPlaCallback callback) {disPlaCallback = callback; }

    void ReceiveCliInfPacket(uint16_t &clientID, ClientInfoPacket& packet ) {
        if (cliInfCallback) cliInfCallback(clientID, packet);
    }
    void ReceiveDisPlaPacket(uint16_t &clientID) {
        if (disPlaCallback) disPlaCallback(clientID);
    }   
private:
    ENetAddress address;
    ENetEvent event;
    ENetHost *server;

    bool serverRunningState = true;

    std::unordered_map<enet_uint32, ENetPeer *> clients; 
    std::vector<ClientInputs> clientsInputBuffer;   

    CliInfCallback cliInfCallback;
    DisPlaCallback disPlaCallback;

    void handleConnections(ENetEvent &event);
    void handleDisconnections(ENetEvent &event);
    
    void sendAllConnectedPlayersPacket(ENetPeer* peer, const std::vector<uint32_t>& playerIDs);
    void sendNewConnectedPlayerPacket(uint16_t &playerID, char username[32]);
    void sendPlayerDisconnectedPacket(uint16_t& playerID);
};

#endif 