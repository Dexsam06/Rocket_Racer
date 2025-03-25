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
    NetworkCommunicator(std::string ipAddress, std::string username);
    ~NetworkCommunicator() {}

    void handleReceivedPacket();
    void disconnectFromServer(); 

    void sendInfoClientPacket(const char *username);
    void sendInputToServer(ClientInputPacket &clientInputPacket); 

    int getClientID() { return clientID; }
    std::string getUsername() {return username; }
    bool getHasReceivedFirstGameState() {return hasReceivedFirstGameState; }
    bool getHasReceivedConnectedPlayers() {return hasReceivedConnectedPlayers; }

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
    std::string username;
    int clientID;

    ConPlaCallback conPlaCallback;
    NewPlaCallback newPlaCallback;
    DisPlaCallback disPlaCallback;

    bool hasReceivedFirstGameState = false;
    bool hasReceivedConnectedPlayers = false;
}; 

#endif