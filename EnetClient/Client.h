
#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H
#include <enet/enet.h>
#include "common.h"
#include "Packet.hpp"

class Client
{
public:
    Client() = default;
    int formConnection();
    Position pos;
    uint8_t health = 3;
    ENetHost* client;
    ENetAddress address; //server's
    ENetEvent event; // recieved from server
    ENetPeer* peer; //server connected to
};

#endif //CLIENT_CLIENT_H