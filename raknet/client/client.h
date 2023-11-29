#pragma once

#include "slikenet/NetworkIDManager.h"
#include "slikenet/peerinterface.h"
#include "../common/game_objects.h"
#include "game/client_gamestate.h"

using namespace SLNet;
class Client
{
public:
    NetworkIDManager manager;
    RakPeerInterface *rakPeer;
    ClientGamestate gamestate;

    Client() = default;
    void WaitForConnectionAndApplyGameConfiguration() const;
    void WaitForPlayerSpawn();
    void StartGameloop();

private:
    void handleLibraryPacket(Packet *p, u_char identifier);
    void handleCustomPacket(Packet *p, u_char identifier);
};