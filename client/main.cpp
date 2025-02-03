#include <iostream>
#include <vector> 
#include <enet/enet.h>
#include <cstring>
#include <bits/this_thread_sleep.h>
#include <signal.h>

struct PlayerInfo {
    char username[32];
} player = {""}; 

bool state = true;

void onExit(int signum) {
    std::cout << "Program is terminating... signal: " << signum << std::endl;
    state = false;
}

int main(int argc, char **argv)
{
    signal(SIGINT, onExit);

    signal(SIGTERM, onExit);

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An Error has occured while initializing enet\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost *client;
    client = enet_host_create(NULL, 1, 1, 0, 0); 

    if (client == NULL)
    {
        fprintf(stderr, "An Error occured while trying to create a ENet client host! \n"); 
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer *serverPeer; 

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 7777;

    std::cout << "Type your username: "; // Type a number and press enter 
    std::cin >> player.username; // Get user input from the keyboard

    serverPeer = enet_host_connect(client, &address, 1, 0);
    if (serverPeer == NULL)
    {
        fprintf(stderr, "No available peers for initalizing an ENet connection!\n");
        return EXIT_FAILURE;
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection to 127.0.0.1:7777 succeeded!");
        ENetPacket* packet = enet_packet_create(&player, sizeof(PlayerInfo), ENET_PACKET_FLAG_RELIABLE);

        // Send the packet to the server
        enet_peer_send(serverPeer, 0, packet);
        enet_host_flush(client); // Immediately send the packet
        std::cout << "Sent player info to server!" << std::endl;
    }
    else
    {
        enet_peer_reset(serverPeer);
        puts("Connection to 127.0.0.1:777 failed!");
        return EXIT_SUCCESS;
    }

    while (state)
    {
        while (enet_host_service(client, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
                       event.packet->dataLength,
                       event.packet->data,
                       event.peer->address.host,
                       event.peer->address.port,
                       event.channelID);
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

    return EXIT_SUCCESS;
}

