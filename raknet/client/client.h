#pragma once

#include "slikenet/NetworkIDManager.h"
#include "slikenet/peerinterface.h"
#include "../common/game_objects.h"
#include "game/client_gamestate.h"
#include "../../common/corrections_tracker.h"

using namespace SLNet;
class Client
{
public:
    RakPeerInterface *rakPeer;
    ClientGamestate gamestate;
    PacketReliability reliabilityMode = UNRELIABLE;
    bool logCorrections = false;
    corrections_tracker* correctionsTracker;

    Client() = default;
    void WaitForConnectionAndApplyGameConfiguration();
    void WaitForPlayerSpawn();
    void StartGameloop(bool display);

private:
    SystemAddress serverAddress;

    void handleLibraryPacket(Packet *p, u_char identifier);
    void handleCustomPacket(Packet *p, u_char identifier);
    void sendLocalPlayerMove(Position newPos);
    void handlePlayerPositionUpdate(NetworkID playerID, Position newPos);
};