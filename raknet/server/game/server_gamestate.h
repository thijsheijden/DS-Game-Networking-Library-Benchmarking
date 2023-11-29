#pragma once

#include <cstdint>
#include "slikenet/types.h"
#include "../common/game_objects.h"
#include "../common/gamestate.h"
#include "slikenet/NetworkIDManager.h"
#include "map"

using namespace std;

class ServerGamestate: public Gamestate {
public:
    map<SystemAddress, Player*> connectedPlayers;

    void CreateAndSpawnNewPlayerForClient(SystemAddress clientAddress);
    void RemovePlayer(SystemAddress clientAddress);
};