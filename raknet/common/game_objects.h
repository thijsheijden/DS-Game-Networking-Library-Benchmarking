#pragma once

#include <cstdint>
#include "slikenet/NetworkIDObject.h"

using namespace SLNet;

#pragma pack(push, 1)
#pragma pack(pop)
struct Position
{
public:
    uint16_t x;
    uint16_t y;

    Position()=default;
    Position(uint16_t initialX, uint16_t initialY)
    {
        x = initialX;
        y = initialY;
    }

    bool operator==(Position & p2) {
        return x == p2.x && y == p2.y;
    }
};

#pragma pack(push, 1)
#pragma pack(pop)
struct Bomb: public NetworkIDObject
{
    Position pos;

    Bomb()= default;
};

#pragma pack(push, 1)
#pragma pack(pop)
class Player: public NetworkIDObject
{
public:
    Position pos;
    uint8_t health = 3;

    Player() = default;
    Player(Position _pos) {
        pos = _pos;
    }
};
