#ifndef NETWORK_PACKETS_H
#define NETWORK_PACKETS_H

#include <cstdint>
#include <cstring>
#include <vector>

#pragma pack(push, 1) // Ensure no padding is added

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
    virtual void Deserialize(const uint8_t *data, size_t size) = 0;
    virtual ~BasePacket() = default;
};

// Packet sent to the connected client, which contain all current connected players
struct ConnectedPlayersPacket : public BasePacket
{
    uint16_t numPlayers;
    std::vector<std::pair<uint32_t, std::string>> clients;
    uint16_t clientID;

    ConnectedPlayersPacket() { type = PacketType::CONNECTED_PLAYERS_PACKET; }
    ConnectedPlayersPacket(const std::vector<std::pair<uint32_t, std::string>> &clients, uint16_t &clientID)
    {
        type = PacketType::CONNECTED_PLAYERS_PACKET;
        numPlayers = static_cast<uint16_t>(clients.size());
        this->clients = clients;
        this->clientID = clientID;
    }

    std::vector<uint8_t> Serialize() const override
    {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(type));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&numPlayers), reinterpret_cast<const uint8_t *>(&numPlayers) + sizeof(uint16_t));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientID), reinterpret_cast<const uint8_t *>(&clientID) + sizeof(uint16_t));

        for (const auto &client : clients)
        {
            uint32_t id = client.first;
            const std::string &name = client.second;
            uint32_t nameLength = name.size(); // Store string length

            // Append ID
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&id), reinterpret_cast<uint8_t *>(&id) + sizeof(uint32_t));

            // Append string length
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&nameLength), reinterpret_cast<uint8_t *>(&nameLength) + sizeof(uint32_t));

            // Append string characters
            buffer.insert(buffer.end(), name.begin(), name.end());
        }

        return buffer;
    }

    void Deserialize(const uint8_t *data, size_t size) override
    {
        if (size < 5) // Minimum size check (1 byte for type, 2 bytes for numPlayers, 2 bytes for clientID)
            return;

        size_t offset = 0;
        type = static_cast<PacketType>(data[offset++]);

        std::memcpy(&numPlayers, &data[offset], sizeof(uint16_t));
        offset += sizeof(uint16_t);

        std::memcpy(&clientID, &data[offset], sizeof(uint16_t));
        offset += sizeof(uint16_t);

        clients.clear(); // Clear existing data before inserting new entries

        for (uint16_t i = 0; i < numPlayers; i++)
        {
            if (offset + sizeof(uint32_t) > size)
                return; // Bounds check for ID
            uint32_t id;
            std::memcpy(&id, &data[offset], sizeof(uint32_t));
            offset += sizeof(uint32_t);

            if (offset + sizeof(uint32_t) > size)
                return; // Bounds check for string length
            uint32_t nameLength;
            std::memcpy(&nameLength, &data[offset], sizeof(uint32_t));
            offset += sizeof(uint32_t);

            if (offset + nameLength > size)
                return; // Bounds check for string data
            std::string name(reinterpret_cast<const char *>(&data[offset]), nameLength);
            offset += nameLength;

            clients.emplace_back(id, name);
        }
    }
};

// Packet that the new connected client sends to the server
struct ClientInfoPacket : public BasePacket
{
    char username[32];

    ClientInfoPacket() { type = PacketType::CLIENT_INFO_PACKET; }
    ClientInfoPacket(const char *name)
    {
        type = PacketType::CLIENT_INFO_PACKET;
        std::strncpy(username, name, sizeof(username));
    }

    std::vector<uint8_t> Serialize() const override
    {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(type));
        buffer.insert(buffer.end(), username, username + 32);
        return buffer;
    }

    void Deserialize(const uint8_t *data, size_t size) override
    {
        if (size < 33)
            return;
        type = static_cast<PacketType>(data[0]);
        std::memcpy(username, &data[1], 32);
    }
};

// Packet that is sent to every connected client that a new client has been added 
struct NewPlayerConnectedPacket : public BasePacket
{
    uint16_t playerID;
    char username[32];

    NewPlayerConnectedPacket() { type = PacketType::NEW_PLAYER_CONNECTED_PACKET; }
    NewPlayerConnectedPacket(uint16_t playerID, const char *name)
    {
        type = PacketType::NEW_PLAYER_CONNECTED_PACKET;
        this->playerID = playerID;
        std::strncpy(username, name, sizeof(username));
    }

    std::vector<uint8_t> Serialize() const override
    {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(type));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&playerID),
                      reinterpret_cast<const uint8_t *>(&playerID) + sizeof(uint16_t));
        buffer.insert(buffer.end(), username, username + 32);
        return buffer;
    }

    void Deserialize(const uint8_t *data, size_t size) override
    {
        if (size < 35)
            return;
        type = static_cast<PacketType>(data[0]);
        std::memcpy(&playerID, &data[1], sizeof(uint16_t));
        std::memcpy(username, &data[3], 32);
    }
};

// Packet that is sent to every connected client that a client has disconnected
struct PlayerDisconnectedPacket : public BasePacket
{
    uint16_t playerID;

    PlayerDisconnectedPacket() { type = PacketType::PLAYER_DISCONNECTED_PACKET; }
    PlayerDisconnectedPacket(uint16_t playerID)
    {
        type = PacketType::PLAYER_DISCONNECTED_PACKET;
        this->playerID = playerID;
    }

    std::vector<uint8_t> Serialize() const override
    {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(type));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&playerID),
                      reinterpret_cast<const uint8_t *>(&playerID) + sizeof(uint16_t));
        return buffer;
    }

    void Deserialize(const uint8_t *data, size_t size) override
    {
        if (size < 3)
            return;
        type = static_cast<PacketType>(data[0]);
        std::memcpy(&playerID, &data[1], sizeof(uint16_t));
    }
};

struct EntityState
{
    uint32_t entityID;
    float posX, posY;
    float rotation;
};

struct ClientState
{
    float serverPosX = 0.0f, serverPosY = 0.0f;
    float rotation = 0.0f;
    bool isThrusting = false, isTurningLeft = false, isTurningRight = false; 
};

// Packet that is sent to every client containing the state of the game
struct GameStatePacket : public BasePacket
{
    uint16_t numEntities;
    std::vector<EntityState> entities;
    ClientState clientState;

    GameStatePacket()
    {
        numEntities = 0;
        type = PacketType::GAME_STATE_PACKET;
    }
    GameStatePacket(const std::vector<EntityState> &entityList, const ClientState &state)
    {
        type = PacketType::GAME_STATE_PACKET;
        numEntities = static_cast<uint16_t>(entityList.size());
        entities = entityList;
        clientState = state;
    }

    std::vector<uint8_t> Serialize() const
    {
        std::vector<uint8_t> buffer;
        buffer.reserve(1 + sizeof(uint16_t) + numEntities * sizeof(EntityState) + sizeof(ClientState));

        // Serialize packet type (1 byte)
        buffer.push_back(static_cast<uint8_t>(type));

        // Serialize number of entities (2 bytes)
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&numEntities), reinterpret_cast<const uint8_t *>(&numEntities) + sizeof(uint16_t));

        // Serialize entities
        for (const auto &entity : entities) 
        {
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entity.entityID), reinterpret_cast<const uint8_t *>(&entity.entityID) + sizeof(uint32_t));

            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entity.posX), reinterpret_cast<const uint8_t *>(&entity.posX) + sizeof(float));

            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entity.posY), reinterpret_cast<const uint8_t *>(&entity.posY) + sizeof(float));

            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entity.rotation), reinterpret_cast<const uint8_t *>(&entity.rotation) + sizeof(float));
        }

        // Serialize client state
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientState.serverPosX), reinterpret_cast<const uint8_t *>(&clientState.serverPosX) + sizeof(float));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientState.serverPosY), reinterpret_cast<const uint8_t *>(&clientState.serverPosY) + sizeof(float));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientState.rotation), reinterpret_cast<const uint8_t *>(&clientState.rotation) + sizeof(float));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientState.isThrusting), reinterpret_cast<const uint8_t *>(&clientState.isThrusting) + sizeof(bool));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientState.isTurningLeft), reinterpret_cast<const uint8_t *>(&clientState.isTurningLeft) + sizeof(bool));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientState.isTurningRight), reinterpret_cast<const uint8_t *>(&clientState.isTurningRight) + sizeof(bool));

        return buffer;
    }

    void Deserialize(const uint8_t *data, size_t size)
    {
        if (size < 3) // Need at least type (1 byte) + numEntities (2 bytes)
        {
            std::cerr << "GameStatePacket: Packet size too small (header missing)!" << std::endl;
            return;
        }

        size_t offset = 0;

        // Skip the first byte (type)
        offset += 1;

        // Read number of entities (2 bytes)
        std::memcpy(&numEntities, &data[offset], sizeof(uint16_t));
        offset += sizeof(uint16_t);

        // Ensure the packet is large enough
        size_t requiredSize = offset + (numEntities * sizeof(EntityState)) + sizeof(ClientState);
        if (size < requiredSize)
        {
            std::cerr << "GameStatePacket: Packet size too small! Expected at least "
                      << requiredSize << " bytes but got " << size << " bytes." << std::endl;
            return;
        }

        // Deserialize entities
        entities.resize(numEntities);
        for (auto &entity : entities)
        {
            std::memcpy(&entity.entityID, &data[offset], sizeof(uint32_t));
            offset += sizeof(uint32_t);

            std::memcpy(&entity.posX, &data[offset], sizeof(float));
            offset += sizeof(float);

            std::memcpy(&entity.posY, &data[offset], sizeof(float));
            offset += sizeof(float);

            std::memcpy(&entity.rotation, &data[offset], sizeof(float));
            offset += sizeof(float);
        }

        // Deserialize client state
        std::memcpy(&clientState.serverPosX, &data[offset], sizeof(float));
        offset += sizeof(float);

        std::memcpy(&clientState.serverPosY, &data[offset], sizeof(float));
        offset += sizeof(float);

        std::memcpy(&clientState.rotation, &data[offset], sizeof(float));
        offset += sizeof(float);

        std::memcpy(&clientState.isThrusting, &data[offset], sizeof(bool));
        offset += sizeof(bool);

        std::memcpy(&clientState.isTurningLeft, &data[offset], sizeof(bool));
        offset += sizeof(bool);

        std::memcpy(&clientState.isTurningRight, &data[offset], sizeof(bool));
        offset += sizeof(bool); 
    }
};

// Packet when sending inputs from the client to the server
struct ClientInputPacket : public BasePacket
{
    int key;
    bool pressed;

    ClientInputPacket() { type = PacketType::INPUT_PACKET; }
    ClientInputPacket(int key, bool pressed)
    {
        type = PacketType::INPUT_PACKET;
        this->key = key; 
        this->pressed = pressed;
    }

    std::vector<uint8_t> Serialize() const override { 
        std::vector<uint8_t> buffer;

        buffer.push_back(static_cast<uint8_t>(type));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&key),
                      reinterpret_cast<const uint8_t *>(&key) + sizeof(int));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&pressed), 
                      reinterpret_cast<const uint8_t *>(&pressed) + sizeof(bool));
        return buffer;
    }
    void Deserialize(const uint8_t *data, size_t size) override {
        size_t offset = 0;
        type = static_cast<PacketType>(data[offset++]);

        std::memcpy(&key, &data[offset], sizeof(int)); 
        offset += sizeof(int);

        std::memcpy(&pressed, &data[offset], sizeof(bool));
    }
};

#pragma pack(pop) // Restore default packing

#endif