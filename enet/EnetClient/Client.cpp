#include <iostream>
#include "Client.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
static int id = 0;

int Client::formConnection()
{
#pragma region InitEnet&Client

    id++;
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
    }
    atexit(enet_deinitialize);

#pragma endregion Init Enet and Client
#pragma region HandleConnection
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 60000;
    client = enet_host_create(NULL, 1, 2,
        0      /* assume any amount of incoming bandwidth */,
        0      /* assume any amount of outgoing bandwidth */);

    if (client == NULL)
    {
        fprintf(stderr,
            "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }
    peer = enet_host_connect(client, &address, 1, 0);

    if (peer == NULL)
    {
        fprintf(stderr, "cant find server");
    }
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connected local");
    }
    else
    {
        enet_peer_reset(peer);
        puts("reset peer local");

    }
#pragma endregion Handle Client Connection
    return 0;
}
