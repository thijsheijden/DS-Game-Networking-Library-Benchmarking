#pragma once

#include "../common/gamestate.h"

class ClientGamestate: public Gamestate {
public:
    void CreateLocalPlayer(NetworkID playerID, Position spawnPos);

private:
    Player localPlayer;
};