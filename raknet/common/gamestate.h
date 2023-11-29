#pragma once

#include "game_objects.h"
#include "slikenet/NetworkIDManager.h"
#include "map"

using namespace std;

class Gamestate {
public:
    NetworkIDManager networkIDManager;
    uint16_t mapWidth;
    uint16_t mapHeight;
    uint16_t numPlayers;

    void ApplyConfig(uint16_t _mapWidth, uint16_t _mapHeight, uint16_t _numPlayers) {
        mapWidth = _mapWidth;
        mapHeight = _mapHeight;
        numPlayers = _numPlayers;
    }
};