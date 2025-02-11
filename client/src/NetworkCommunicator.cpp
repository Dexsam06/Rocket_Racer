
#include "NetworkCommunicator.hpp"

bool NetworkCommunicator::connected = true;

NetworkCommunicator::NetworkCommunicator(std::string &ipAddress)
{
    signal(SIGINT, onExit);

    signal(SIGTERM, onExit);

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An Error has occured while initializing enet\n");
        return;
    }
    atexit(enet_deinitialize);

    client = enet_host_create(NULL, 1, 1, 0, 0);

    if (client == NULL)
    {
        fprintf(stderr, "An Error occured while trying to create a ENet client host! \n");
        return;
    }

    enet_address_set_host(&address, ipAddress.c_str());
    address.port = 7777;

    std::cout << "Type your username: ";
    std::cin >> connectData.username;

    serverPeer = enet_host_connect(client, &address, 1, 0);
    if (serverPeer == NULL)
    {
        fprintf(stderr, "No available peers for initalizing an ENet connection!\n");
        return;
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connection to " << address.host << ":7777 succeeded!" << std::endl;
        ENetPacket *packet = enet_packet_create(&connectData, sizeof(connectData), ENET_PACKET_FLAG_RELIABLE);

        // Send the packet to the server
        enet_peer_send(serverPeer, 0, packet);
        enet_host_flush(client); // Immediately send the packet
        std::cout << "Sent player info to server!" << std::endl;
    }
    else
    {
        enet_peer_reset(serverPeer);
        std::cout << "Connection to " << address.host << ":7777 failed!" << std::endl;
        return;
    }
}

NetworkCommunicator::~NetworkCommunicator() {}

void NetworkCommunicator::NetworkHandler()
{
    while (connected)
    {
        while (enet_host_service(client, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                if (!event.packet || event.packet->dataLength < sizeof(EntityData))
                {
                    std::cerr << "Invalid packet received!" << std::endl;
                    return;
                }

                size_t entityCount = event.packet->dataLength / sizeof(EntityData);
                EntityData *dataArray = reinterpret_cast<EntityData *>(event.packet->data);

                std::vector<EntityData> receivedEntities;
                for (size_t i = 0; i < entityCount; i++)
                {
                    receivedEntities.push_back(dataArray[i]);
                }

                if (entityCallback)
                {
                    entityCallback(receivedEntities);
                }

                enet_packet_destroy(event.packet);
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    enet_peer_disconnect(serverPeer, 0);

    while (enet_host_service(client, &event, 3000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            puts("Disconnection succeeded!");
            break;
        }
    }
}

void NetworkCommunicator::sendInputToServer(NetworkCommunicator::PlayerInputs inputs)
{
    ENetPacket *packet = enet_packet_create(&inputs, sizeof(inputs), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(serverPeer, 0, packet);
    enet_host_flush(client);
    enet_packet_destroy(packet);
}