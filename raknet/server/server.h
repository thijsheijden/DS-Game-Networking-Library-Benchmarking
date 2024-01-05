#pragma once

#include "game/server_gamestate.h"
#include "slikenet/peerinterface.h"

using namespace SLNet;
using namespace std;

class Server {
public:
    RakPeerInterface *rakPeer;
    ServerGamestate gamestate;
    PacketReliability reliabilityMode = UNRELIABLE;

    void StartGameLoop();

private:
    void handleLibraryPacket(Packet *p, u_char identifier);
    void handleCustomPacket(Packet *p, u_char identifier);
    void sendGameConfigToClient(SystemAddress address) const;
    void sendPlayerSpawnAndIDToClient(SystemAddress address);
    void broadcastNewPlayerJoinedMessage(Player *player);
};