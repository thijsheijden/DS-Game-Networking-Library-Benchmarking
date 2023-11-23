#include <cstdint>
#include "common.h"
#include "slikenet/NetworkIDObject.h"

#ifndef CLIENT_PLAYER_H
#define CLIENT_PLAYER_H

using namespace SLNet;
class Player: public NetworkIDObject
{
public:
    Position pos;
    uint8_t health = 3;

    Player() = default;
};

#endif //CLIENT_PLAYER_H
