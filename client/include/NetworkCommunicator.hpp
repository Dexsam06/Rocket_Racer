#ifndef NETWORKCOMMUNICATOR_HPP
#define NETWORKCOMMUNICATOR_HPP

#include <enet/enet.h>
#include <iostream>
#include <vector>
#include <functional>

#include "GameController.hpp"
#include "NetworkPackets.hpp"

class NetworkCommunicator
{
public:
    NetworkCommunicator();
    ~NetworkCommunicator();

    void handleReceivedPacket();
    void disconnectFromServer();

    void sendInfoClientPacket(const char *username);
    void sendInputPacketToServer(std::vector<InputWithSequence> &inputList);

    int getClientID() { return clientID; }

    GameStatePacket &getGameStatePacket() { return gameStatePacket; }
    GameStatePacket gameStatePacket;

    using ConPlaCallback = std::function<void(ConnectedPlayersPacket &)>;
    using NewPlaCallback = std::function<void(NewPlayerConnectedPacket &)>;
    using DisPlaCallback = std::function<void(PlayerDisconnectedPacket &)>;

    void setConPlaCallback(ConPlaCallback callback) {conPlaCallback = callback; }
    void setNewPlaCallback(NewPlaCallback callback) {newPlaCallback = callback; }
    void setDisPlaCallback(DisPlaCallback callback) {disPlaCallback = callback; }

    void ReceiveConPlaPacket(ConnectedPlayersPacket& packet) {
        if (conPlaCallback) conPlaCallback(packet);
    }
    void ReceiveNewPlaPacket(NewPlayerConnectedPacket& packet) {
        if (newPlaCallback) newPlaCallback(packet);
    }
    void ReceiveDisPlaPacket(PlayerDisconnectedPacket& packet) {
        if (disPlaCallback) disPlaCallback(packet);
    }   

private:
    ENetAddress address;
    ENetEvent event;
    ENetPeer *serverPeer;
    ENetHost *client;

    std::string ipAddress;
    char username[32];
    int clientID;

    ConPlaCallback conPlaCallback;
    NewPlaCallback newPlaCallback;
    DisPlaCallback disPlaCallback;
};

#endif