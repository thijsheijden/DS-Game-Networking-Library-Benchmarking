#pragma once

#include "../common/gamestate.h"
#include "../common/network_messages.h"
#include "vector"
#include "ncurses.h"

using namespace std;

class ClientGamestate: public Gamestate {
public:
    // The player this client controls
    Player* localPlayer;
    // The remote player positions, include the network ID to easily remove a player position when the player leaves the game
    vector<pair<NetworkID, Position*>> remotePlayerPositions;

    void CreateLocalPlayer(NetworkID playerID, Position spawnPos);
    void AddRemotePlayer(NetworkID remotePlayerID, Position pos);
    Position PerformLocalMove();
    void Display();
};