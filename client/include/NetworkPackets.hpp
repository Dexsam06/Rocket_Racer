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
    uint32_t lastVerifiedInputID = 0;
    float serverPosX = 0.0f, serverPosY = 0.0f;
    float rotation = 0.0f;
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
        std::array<uint8_t, sizeof(uint16_t)> tempBuffer16;
        std::memcpy(tempBuffer16.data(), &numEntities, sizeof(uint16_t));
        buffer.insert(buffer.end(), tempBuffer16.begin(), tempBuffer16.end());

        // Temporary buffers
        std::array<uint8_t, sizeof(uint32_t)> tempBuffer32;
        std::array<uint8_t, sizeof(float)> tempBufferFloat;

        // Serialize entities
        for (const auto &entity : entities)
        {
            std::memcpy(tempBuffer32.data(), &entity.entityID, sizeof(uint32_t));
            buffer.insert(buffer.end(), tempBuffer32.begin(), tempBuffer32.end());

            std::memcpy(tempBufferFloat.data(), &entity.posX, sizeof(float));
            buffer.insert(buffer.end(), tempBufferFloat.begin(), tempBufferFloat.end());

            std::memcpy(tempBufferFloat.data(), &entity.posY, sizeof(float));
            buffer.insert(buffer.end(), tempBufferFloat.begin(), tempBufferFloat.end());

            std::memcpy(tempBufferFloat.data(), &entity.rotation, sizeof(float));
            buffer.insert(buffer.end(), tempBufferFloat.begin(), tempBufferFloat.end());
        }

        // Serialize client state
        std::memcpy(tempBuffer32.data(), &clientState.lastVerifiedInputID, sizeof(uint32_t));
        buffer.insert(buffer.end(), tempBuffer32.begin(), tempBuffer32.end());

        std::memcpy(tempBufferFloat.data(), &clientState.serverPosX, sizeof(float));
        buffer.insert(buffer.end(), tempBufferFloat.begin(), tempBufferFloat.end());

        std::memcpy(tempBufferFloat.data(), &clientState.serverPosY, sizeof(float));
        buffer.insert(buffer.end(), tempBufferFloat.begin(), tempBufferFloat.end());

        std::memcpy(tempBufferFloat.data(), &clientState.rotation, sizeof(float));
        buffer.insert(buffer.end(), tempBufferFloat.begin(), tempBufferFloat.end());

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
        std::memcpy(&clientState.lastVerifiedInputID, &data[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(&clientState.serverPosX, &data[offset], sizeof(float));
        offset += sizeof(float);

        std::memcpy(&clientState.serverPosY, &data[offset], sizeof(float));
        offset += sizeof(float);

        std::memcpy(&clientState.rotation, &data[offset], sizeof(float));
        offset += sizeof(float);
    }
};

struct KeyInput
{
    int keyCode;
    float duration;
};

// Packet when sending inputs from the client to the server
struct InputWithSequence : public BasePacket
{
    int sequenceNumber;
    std::vector<KeyInput> keyInputPacket;

    InputWithSequence() { type = PacketType::INPUT_PACKET; }
    InputWithSequence(int &inputSequenceNumber, std::vector<KeyInput> &keyInput)
    {
        type = PacketType::INPUT_PACKET;
        sequenceNumber = inputSequenceNumber;
        keyInputPacket = keyInput;
    }

    std::vector<uint8_t> Serialize() const override { return {}; }
    void Deserialize(const uint8_t *data, size_t size) override {}

    // Serialize a vector of InputWithSequence into a byte buffer
    static std::vector<uint8_t> Serialize(const std::vector<InputWithSequence> &inputList)
    {
        std::vector<uint8_t> buffer;

        buffer.push_back(static_cast<uint8_t>(PacketType::INPUT_PACKET)); // Packet type

        // First, serialize the number of InputWithSequence packets
        int numInputWithSequence = static_cast<int>(inputList.size());
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&numInputWithSequence),
                      reinterpret_cast<const uint8_t *>(&numInputWithSequence) + sizeof(int));

        // Now serialize each InputWithSequence object
        for (const auto &input : inputList)
        {
            // Serialize the sequence number
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&input.sequenceNumber),
                          reinterpret_cast<const uint8_t *>(&input.sequenceNumber) + sizeof(int));

            // Serialize the number of key inputs
            int numInputs = static_cast<int>(input.keyInputPacket.size());
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&numInputs),
                          reinterpret_cast<const uint8_t *>(&numInputs) + sizeof(int));

            // Serialize all key inputs
            for (const auto &keyInput : input.keyInputPacket)
            {
                buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&keyInput.keyCode),
                              reinterpret_cast<const uint8_t *>(&keyInput.keyCode) + sizeof(int));
                buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&keyInput.duration),
                              reinterpret_cast<const uint8_t *>(&keyInput.duration) + sizeof(float));
            }
        }

        return buffer;
    }

    // Deserialize a vector of InputWithSequence from a byte buffer
    static void Deserialize(const uint8_t *data, size_t size, std::vector<InputWithSequence> &inputList)
    {
        if (size < 1 + sizeof(int)) // Ensure there's enough data for at least packet type and number of inputs
            return;

        size_t offset = 1; // Skip the first byte since it contains the PacketType (already used in switch-case)

        // First, deserialize the number of InputWithSequence packets
        int numInputWithSequence;
        std::memcpy(&numInputWithSequence, &data[offset], sizeof(int));
        offset += sizeof(int);

        if (size < offset + numInputWithSequence * (sizeof(int) + sizeof(int))) // Basic size check
            return;

        // Deserialize each InputWithSequence object
        inputList.resize(numInputWithSequence);
        for (auto &input : inputList)
        {
            // Deserialize the sequence number
            std::memcpy(&input.sequenceNumber, &data[offset], sizeof(int));
            offset += sizeof(int);

            // Deserialize the number of key inputs
            int numInputs;
            std::memcpy(&numInputs, &data[offset], sizeof(int));
            offset += sizeof(int);

            if (size < offset + numInputs * (sizeof(int) + sizeof(float))) // Check size before reading inputs
                return;

            // Resize the keyInputPacket vector and deserialize each KeyInput
            input.keyInputPacket.resize(numInputs);
            for (int i = 0; i < numInputs; ++i)
            {
                std::memcpy(&input.keyInputPacket[i].keyCode, &data[offset], sizeof(int));
                offset += sizeof(int);
                std::memcpy(&input.keyInputPacket[i].duration, &data[offset], sizeof(float));
                offset += sizeof(float);
            }
        }
    }
};

#pragma pack(pop) // Restore default packing

#endif