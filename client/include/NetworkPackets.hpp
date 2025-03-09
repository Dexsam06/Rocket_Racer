#ifndef NETWORK_PACKETS_H
#define NETWORK_PACKETS_H

#include <cstdint>
#include <cstring>
#include <vector>

enum class PacketType : uint8_t 
{
    CONNECTED_PLAYERS_PACKET = 1, 
    CLIENT_INFO_PACKET = 2,
    NEW_PLAYER_CONNECTED_PACKET = 3,
    PLAYER_DISCONNECTED_PACKET = 4,
    GAME_STATE_PACKET = 5,
    INPUT_PACKET = 6
};

// Base packet class 
struct BasePacket 
{
    PacketType type; 

    virtual std::vector<uint8_t> Serialize() const = 0;  
    virtual void Deserialize(const uint8_t* data, size_t size) = 0; 
    virtual ~BasePacket() = default;
};

// Packet sent to the connected client, which contain all current connected players
struct ConnectedPlayersPacket : public BasePacket 
{
    uint16_t numPlayers; 
    std::vector<uint32_t> playerIDs; 
    
    ConnectedPlayersPacket() { type = PacketType::CONNECTED_PLAYERS_PACKET; }
    ConnectedPlayersPacket(const std::vector<uint32_t>& ids) {
        type = PacketType::CONNECTED_PLAYERS_PACKET;
        numPlayers = static_cast<uint16_t>(ids.size());
        playerIDs = ids;
    }

    std::vector<uint8_t> Serialize() const override {
        std::vector<uint8_t> buffer;
        
        buffer.reserve(sizeof(numPlayers) + playerIDs.size() * sizeof(uint32_t));

        uint16_t num = numPlayers;
        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&num), reinterpret_cast<uint8_t*>(&num) + sizeof(uint16_t));

        for (uint32_t id : playerIDs) {
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&id), reinterpret_cast<uint8_t*>(&id) + sizeof(uint32_t));
        }

        return buffer;
    }

    void Deserialize(const uint8_t* data, size_t size) override {
        if (size < sizeof(uint16_t)) return; 

        std::memcpy(&numPlayers, data, sizeof(uint16_t)); 

        size_t expectedSize = sizeof(uint16_t) + numPlayers * sizeof(uint32_t);
        if (size < expectedSize) return;  

        playerIDs.resize(numPlayers); 
        std::memcpy(playerIDs.data(), data + sizeof(uint16_t), numPlayers * sizeof(uint32_t));
    }
};

// Packet that the new connected client sends to the server
struct ClientInfoPacket : public BasePacket 
{
    char username[32]; 

    ClientInfoPacket() { type = PacketType::CLIENT_INFO_PACKET; }
    ClientInfoPacket(const char* name) {
        type = PacketType::CLIENT_INFO_PACKET;
        std::strncpy(username, name, sizeof(username));
    }

    std::vector<uint8_t> Serialize() const override {
        std::vector<uint8_t> buffer(sizeof(ClientInfoPacket)); 
        std::memcpy(buffer.data(), this, sizeof(ClientInfoPacket));
        return buffer;
    }

    void Deserialize(const uint8_t* data, size_t size) override {
        if (size >= sizeof(ClientInfoPacket)) {
            std::memcpy(this, data, sizeof(ClientInfoPacket));
        }
    }
};

// Packet that is sent to every connected client that a new client has been added
struct NewPlayerConnectedPacket : public BasePacket 
{
    uint32_t playerID;

    NewPlayerConnectedPacket() { type = PacketType::NEW_PLAYER_CONNECTED_PACKET; }
    NewPlayerConnectedPacket(uint32_t &playerID) {
        type = PacketType::NEW_PLAYER_CONNECTED_PACKET;
        this->playerID = playerID;
    }

    std::vector<uint8_t> Serialize() const override {
        std::vector<uint8_t> buffer(sizeof(NewPlayerConnectedPacket)); 
        std::memcpy(buffer.data(), this, sizeof(NewPlayerConnectedPacket));
        return buffer;
    }

    void Deserialize(const uint8_t* data, size_t size) override {
        if (size >= sizeof(NewPlayerConnectedPacket)) {
            std::memcpy(this, data, sizeof(NewPlayerConnectedPacket));
        }
    }
};

// Packet that is sent to every connected client that a client has disconnected
struct PlayerDisconnectedPacket : public BasePacket
{
    uint32_t playerID;

    PlayerDisconnectedPacket() { type = PacketType::PLAYER_DISCONNECTED_PACKET; }
    PlayerDisconnectedPacket(uint32_t &playerID) {
        type = PacketType::PLAYER_DISCONNECTED_PACKET;
        this->playerID = playerID;
    }

    std::vector<uint8_t> Serialize() const override {
        std::vector<uint8_t> buffer(sizeof(PlayerDisconnectedPacket)); 
        std::memcpy(buffer.data(), this, sizeof(PlayerDisconnectedPacket));
        return buffer;
    }

    void Deserialize(const uint8_t* data, size_t size) override {
        if (size >= sizeof(PlayerDisconnectedPacket)) {
            std::memcpy(this, data, sizeof(PlayerDisconnectedPacket));
        }
    }
};

struct EntityState {
    uint32_t entityID;
    float posX, posY;
    float rotation;
};

struct ClientState {
    uint32_t lastVerifiedInputID;
    float serverPosX, serverPosY;
    float rotation;
};

// Packet that is sent to every client containing the state of the game
struct GameStatePacket : public BasePacket
{
    uint16_t numEntities;
    std::vector<EntityState> entities;
    ClientState clientState;

    GameStatePacket() {type = PacketType::GAME_STATE_PACKET; }
    GameStatePacket(const std::vector<EntityState>& entityList, const ClientState& state) { 
        numEntities = static_cast<uint16_t> (entityList.size());
        entities = entityList;
        clientState = state;
    }

    std::vector<uint8_t> Serialize() const {
        std::vector<uint8_t> buffer;

        buffer.reserve(sizeof(uint16_t) + (entities.size() * sizeof(EntityState)) + sizeof(ClientState));

        uint16_t num = numEntities;
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&num), reinterpret_cast<const uint8_t*>(&num) + sizeof(uint16_t));

        for (const auto& entity : entities) { 
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&entity), reinterpret_cast<const uint8_t*>(&entity) + sizeof(EntityState));
        }

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&clientState), reinterpret_cast<const uint8_t*>(&clientState) + sizeof(ClientState));

        return buffer;
    }

    void Deserialize(const uint8_t* data, size_t size) {
        if (size < sizeof(uint16_t)) return; 

        size_t offset = 0;

        std::memcpy(&numEntities, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        size_t expectedSize = sizeof(uint16_t) + numEntities * sizeof(EntityState) + sizeof(ClientState); 
        if (size < expectedSize) return;  

        entities.resize(numEntities);  
        std::memcpy(entities.data(), data + offset, numEntities * sizeof(EntityState));
        offset += numEntities * sizeof(EntityState);

        std::memcpy(&clientState, data + offset, sizeof(ClientState));
    } 
};

struct KeyInput {
    int keyCode;
    float duration;
};

// Packet when sending inputs from the client to the server
struct InputWithSequence : public BasePacket {
    int sequenceNumber;
    std::vector<KeyInput> keyInputPacket;

    InputWithSequence() { type = PacketType::INPUT_PACKET; } 

    // Serialize a vector of InputWithSequence into a byte buffer
    static std::vector<uint8_t> Serialize(const std::vector<InputWithSequence>& inputList) {
        std::vector<uint8_t> buffer;

        // First, serialize the number of InputWithSequence packets
        int numInputWithSequence = inputList.size();
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&numInputWithSequence), reinterpret_cast<const uint8_t*>(&numInputWithSequence) + sizeof(int));

        // Now serialize each InputWithSequence object
        for (const auto& input : inputList) {
            // Serialize the sequence number
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&input.sequenceNumber), reinterpret_cast<const uint8_t*>(&input.sequenceNumber) + sizeof(int));

            // Serialize the number of key inputs
            int numInputs = static_cast<int>(input.keyInputPacket.size());
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&numInputs), reinterpret_cast<const uint8_t*>(&numInputs) + sizeof(int));

            // Serialize all key inputs
            for (const auto& keyInput : input.keyInputPacket) {
                buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&keyInput.keyCode), reinterpret_cast<const uint8_t*>(&keyInput.keyCode) + sizeof(int));
                buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&keyInput.duration), reinterpret_cast<const uint8_t*>(&keyInput.duration) + sizeof(float));
            }
        }

        return buffer;
    }

    // Deserialize a vector of InputWithSequence from a byte buffer
    static void Deserialize(const uint8_t* data, size_t size, std::vector<InputWithSequence>& inputList) {
        size_t offset = 0;

        // First, deserialize the number of InputWithSequence packets
        int numInputWithSequence;
        std::memcpy(&numInputWithSequence, data + offset, sizeof(int));
        offset += sizeof(int);

        // Deserialize each InputWithSequence object
        inputList.resize(numInputWithSequence);
        for (auto& input : inputList) {
            // Deserialize the sequence number
            std::memcpy(&input.sequenceNumber, data + offset, sizeof(int));
            offset += sizeof(int);

            // Deserialize the number of key inputs
            int numInputs;
            std::memcpy(&numInputs, data + offset, sizeof(int));
            offset += sizeof(int);

            // Resize the keyInputPacket vector and deserialize each KeyInput
            input.keyInputPacket.resize(numInputs);
            for (int i = 0; i < numInputs; ++i) {
                // Deserialize each keyInput
                std::memcpy(&input.keyInputPacket[i].keyCode, data + offset, sizeof(int));
                offset += sizeof(int);
                std::memcpy(&input.keyInputPacket[i].duration, data + offset, sizeof(float));
                offset += sizeof(float);
            }
        }
    }
};

#endif 